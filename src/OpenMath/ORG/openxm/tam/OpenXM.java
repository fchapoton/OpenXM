/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;


/**
 * OpenXM �����ФȤ���³��Ԥʤ���ݥ��饹
 * ���饤�����¦�����Ѥ���.
 */
public class OpenXM{
  private OpenXMconnection control = null, stream = null;
  final protected boolean debug = false;

  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMconnection(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMconnection(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  /**
   * ��³�� reset ��Ԥʤ�. ���ߤ�̤����.
   */
  public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    debug("control: sending SYNC BALL.");
  }

  /**
   * OpenXM ��å���������������. �����������Υ᥽�åɤ�
   * �ܥǥ�����ʬ�����Ǥ褤. �إå�����ʬ�ϼ�ư���ղä����.
  public void send(OXbody object) throws IOException,MathcapViolation{
    stream.send(object);
  }

  /**
   * OpenXM ��å�������������.
   */
  public OXmessage receive() throws IOException{
    return stream.receive();
  }

  /**
   * MathCap �� mathcap �����ꤹ��.
   * �ʸ�, mathcap ��ȿ���� CMO �������
   * MathcapViolation ��ȯ������.
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
        System.out.println("");
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

    System.out.println("breaking...");

    try{
      // close connection
      ox.stream.close();
      ox.control.close();
    }catch(IOException e){
      e.printStackTrace();
    }
  }
}
