/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXM.java,v 1.8 2001/11/04 09:58:14 takayama Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;


/*&ja
    OpenXM �����ФȤ���³��Ԥʤ����饹. 
    ���饤�����¦�����Ѥ���.     
    ��³���륵������˰�Ĥ� OpenXM ���饹��ɬ��. 
*/
/**
 * OpenXM is a class to connect to OpenXM servers,
 * which are compliant to OpenXM RFC 100.
 * There is one-to-one correspondence between the instances
 * of the class OpenXM and the OpenXM servers.
 */
public class OpenXM{
  private OpenXMstream control = null, stream = null;
  final protected boolean debug = false;

  /*&ja
   * OpenXM �����ФȤ���³�� TCP/IP �����åȤ��Ѥ��ƹԤʤ�.
   * �ޥ���̾ host �Υݡ����ֹ� CtrlPort �˥���ȥ����,
   * �ݡ����ֹ� StreamPort �˥ǡ����Ѥ���³��Ԥʤ�.
   */
  /**
   * Connect to an OpenXM server via TCP/IP socket.
   * @param host  a machine name of the OpenXM server.
   * @param CtrlPort  the control port number of the OpenXM server.
   * @param StreamPort  the data port number of the OpenXM server.
   * As to details on the notion of control port and data port, see 
   *    Design and Implementation of OpenXM client server model and
   *        common mathematical object format (OpenXM-RFC 100,
   *                                            proposed standard) 
   *   @see <a href="http://www.openxm.org">OpenXM</a>
   */
  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  /*&ja
   * ���ޥ�� command ��Ω���夲��
   * OpenXM �����ФȤ���³�� TCP/IP �����åȤ��Ѥ��ƹԤʤ�.
   * �ޥ���̾ host �Υݡ����ֹ� CtrlPort �˥���ȥ����,
   * �ݡ����ֹ� StreamPort �˥ǡ����Ѥ���³��Ԥʤ�.
   */
  /**
   * First, execute a command, which is usually an OpenXM server,
   * and next try to connect to the OpenXM server via TCP/IP.
   * @param command  a command.
   * @param host  a machine name of the OpenXM server.
   * @param CtrlPort  the control port number of the OpenXM server.
   * @param StreamPort  the data port number of the OpenXM server.
   */
  public OpenXM(String command,String host,int CtrlPort,int StreamPort)
       throws IOException{
    Runtime.getRuntime().exec(command);
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  public OpenXM(String host,int CtrlPort,int StreamPort,String ox_server,int oxd_port,String pass)
	   throws IOException{
		 // pass may be a null string.
    control = new OpenXMstream(host,CtrlPort,pass);
    stream = new OpenXMstream(host,StreamPort,pass);
    System.err.println("Listening...");
    System.err.println("Launch ox server with the reverse option, e.g., ox -ox ox_asir -reverse");
    // Launch ox_server by the oxd daemon (oxd_port).
    // BUG: It has not yet been implemented.
    control.OpenXMstreamAccept();
    System.err.println("Accepted the control port.");
    stream.OpenXMstreamAccept();
    System.err.println("Accepted the data port.");


    control.sendByteOrder();
    stream.sendByteOrder();
  }
  /*&ja
   * �����Фη׻����Ǥ�Ԥʤ�. ���ߤ�̤����.
   */
  /**
   * Resetting the engine process.  It has not yet been implemented.
   */
  public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    debug("control: sending SYNC BALL.");
  }

  /*&ja
   * OpenXM ��å�������ǡ������ȥ꡼�����������.
   * ���Υ᥽�åɤϥ�å������Υܥǥ�����ʬ�����Ǥ褤.
   * �إå���ʬ�ϼ�ư���ղä����.
   */
  /**
   * Send an OpenXM message object. 
   * @param object a message. For example, oxm.send(new CMO_STRING("Hello"))
   * sends a string "Hello" to the OpenXM server oxm in the CMO_STRING data
   * encoding.
   */
  public void send(OXbody object) throws IOException,MathcapViolation{
    stream.send(object);
  }

  /*&ja
   * �ǡ������ȥ꡼�फ�� OpenXM ��å�������������.
   */
  /**
   * Receive an OpenXM message.
   */
  public OXmessage receive() throws IOException{
    return stream.receive();
  }

  /*&ja
   * �ǡ������ȥ꡼��� MathCap �� mathcap �����ꤹ��.
   * �ʸ�, �������륪�֥������Ȥ� mathcap �˹�äƤ��뤫�ɤ���
   * �����å�������. �ºݤ˥����å������뤫�ɤ�����
   * OXbody ���饹���������饹�μ����ˤ��.
   * mathcap ��ȿ�������֥������Ȥ������Ȥ������ˤ�,
   * �ʸ� MathcapViolation ��ȯ�����뤳�Ȥ����Ԥ����.
   */
  /**
   * Set the mathcap.
   * If one tries to send an object which is prohibited to send by the mathcap,
   * the mathcapViolation exception is thrown.
   */
  public void setMathCap(CMO_MATHCAP mathcap){
    stream.setMathCap(mathcap);
  }

  private final void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    Runnable process = null;
    Thread thread;
    OpenXM ox;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.err.println("");
        System.exit(0);
      }else if(argv[i].equals("-host")){
        hostname = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option :"+ argv[i]);
        System.exit(1);
      }
    }

    try{
      ox = new OpenXM(hostname,ControlPort,DataPort);

      thread = new Thread(process);
      thread.start();
    }catch(UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }

    try{
      //������¦��ʸ������������ޤ���
      ox.send(new SM(SM.SM_mathcap));
      ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("print(\"Hello world!!\");\n"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));

      //ox.send(new CMO_STRING("def sub(B,A){return B-A;}"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("diff((x+2*y)^2,x);\0 1+2;"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_ZZ("1"));
      ox.send(new CMO_ZZ("-2"));
      //ox.send(new CMO_INT32(2));
      //ox.send(new CMO_STRING("sub"));
      //ox.send(new SM(SM.SM_executeFunction));
      ox.send(new SM(SM.SM_popCMO));

      { int[] array = {1,2};
      ox.send(new CMO_MONOMIAL32(array,new CMO_ZZ("-2")));
      }
      ox.send(new SM(SM.SM_popCMO));

      ox.send(new SM(SM.SM_popString));
      //ox.send(new SM(SM.SM_popString));

      ox.send(new SM(SM.SM_control_kill));

      //os.flush();

      //������¦�����������줿ʸ�����������ޤ���
      while(true){
	ox.receive();
      }

    }catch(Exception e){
      e.printStackTrace();
    }

    System.err.println("breaking...");

    try{
      // close connection
      ox.stream.close();
      ox.control.close();
    }catch(IOException e){
      e.printStackTrace();
    }
  }
}
