/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.java,v 1.5 1999/11/10 21:25:48 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

class OpenXMconnection{
  protected int serial_num = 0x12345678;
  Socket socket = null;
  InputStream  is = null;
  OutputStream os = null;
  int order = OX_BYTE_NETWORK_BYTE_ORDER;

  // OX message_tag
  final public static int OX_COMMAND                  = 513;
  final public static int OX_DATA                     = 514;
  final public static int OX_SYNC_BALL                = 515;
  final public static int OX_DATA_WITH_LENGTH         = 521;
  final public static int OX_DATA_OPENMATH_XML        = 523;
  final public static int OX_DATA_OPENMATH_BINARY     = 524;
  final public static int OX_DATA_MP                  = 525;
  final public static int OX_PRIVATE                  = 0x7fff0000;

  // byte order (support network byte order only)
  final public static int OX_BYTE_NETWORK_BYTE_ORDER = 0;
  final public static int OX_BYTE_LITTLE_INDIAN      = 1;
  final public static int OX_BYTE_BIG_INDIAN         = 0xff;

  public OpenXMconnection(String host,int Port) throws IOException{
    this(host,Port,false);
  }

  public OpenXMconnection(String host,int Port,boolean reverse) throws IOException{
    // create socket
    if(!reverse){
      socket = new Socket(host,Port);
    }else{
      socket = new ServerSocket(Port,50,InetAddress.getByName(host)).accept();
    }

    is = new BufferedInputStream(socket.getInputStream());
    os = socket.getOutputStream();
  }

  public int sendByteOrder() throws IOException{
    // send byte order
    os.write(OX_BYTE_NETWORK_BYTE_ORDER);
    os.flush();

    // receive byte order
    //System.out.println("get: "+is.read());
    is.read();

    order = OX_BYTE_NETWORK_BYTE_ORDER;

    return order;
  }

  public void close() throws IOException{
    is.close();
    os.close();
    socket.close();
  }

  public void sendSM(SM code) throws IOException{
    DataOutputStream dos = new DataOutputStream(os);

    dos.writeInt(OX_COMMAND);
    dos.writeInt(serial_num);
    code.send(dos);
    os.flush();
  }

  public void sendCMO(CMO object) throws IOException{
    DataOutputStream dos = new DataOutputStream(os);

    dos.writeInt(OX_DATA);
    dos.writeInt(serial_num);
    object.send(dos);
    dos.flush();
  }

  public void send(Object object) throws IOException{
    if(object instanceof CMO){
      sendCMO((CMO)object);
    }else if(object instanceof SM){
      sendSM((SM)object);
    }
  }

  public void sendOX_SYNC_BALL() throws IOException{
    DataOutputStream dos = new DataOutputStream(os);

    dos.writeInt(OX_SYNC_BALL);
    dos.writeInt(serial_num);
  }

  public CMO receiveCMO() throws IOException{
    return CMO.receive(new DataInputStream(new DebugInputStream(is)));
  }

  public SM receiveSM() throws IOException{
    return new SM(is);
  }

  public int receiveOXtag() throws IOException{
    DataInputStream dis = new DataInputStream(is);
    int tag = dis.readInt();

    dis.readInt(); // read serial

    return tag;
  }

  public void receive() throws IOException{
    DataInputStream dis = new DataInputStream(is);
    int tag = receiveOXtag();

    System.out.print("("+ toString(tag) +",");
    //System.out.print("serial="+ dis.readInt() +"," );

    switch(tag){
    case OX_COMMAND:
      System.out.print(""+ receiveSM());
      break;

    case OX_DATA:
      System.out.print(""+ receiveCMO());
      break;
    }

    System.out.println(")");
  }

  public static String toString(int tag){
    switch(tag){
    case OX_COMMAND:
      return "OX_COMMAND";

    case OX_DATA:
      return "OX_DATA";

    case OX_SYNC_BALL:
      return "OX_SYNC_BALL";
    }

    return "";
  }
}
