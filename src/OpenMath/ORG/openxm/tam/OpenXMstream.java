/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXMstream.java,v 1.1 2000/09/08 07:29:45 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;

/**
 * OpenXM $B%a%C%;!<%8$NAw<u?.$r9T$J$&%/%i%9(B.
 * $B%3%s%H%m!<%k(B, $B%G!<%?$=$l$>$l$K0l$D$:$DI,MW$H$J$k(B.
 * $B$3$N%/%i%9$G$O(B, $B@\B3;~$K%P%$%H%*!<%@$N8r49$r9T$J$$(B,
 * 32bit $B@0?t$NAw<u?.;~$K%P%$%H%*!<%@$NJQ49$r9T$J$&(B.
 * $B8=:_$O%M%C%H%o!<%/%P%$%H%*!<%@$N$_<BAu$5$l$F$$$k(B.
 */
public class OpenXMstream{
  private int serial = 0;
  private Socket socket = null;
  private InputStream  is = null;
  private OutputStream os = null;
  private int order = OX_BYTE_NETWORK_BYTE_ORDER;
  private CMO_MATHCAP mathcap = null;
  private ByteArrayOutputStream buffer = new ByteArrayOutputStream();

  /**
   * $B%M%C%H%o!<%/%P%$%H%*!<%@$rI=$9(B.
   * $B%P%$%H%*!<%@$N7hDj;~$KMQ$$$k(B.
   */
  final public static int OX_BYTE_NETWORK_BYTE_ORDER = 0;

  /**
   * $B%j%H%k%$%s%G%#%"%s$rI=$9(B.
   * $B%P%$%H%*!<%@$N7hDj;~$KMQ$$$k(B.
   */
  final public static int OX_BYTE_LITTLE_INDIAN      = 1;

  /**
   * $B%S%C%0%$%s%G%#%"%s$rI=$9(B.
   * $B%P%$%H%*!<%@$N7hDj;~$KMQ$$$k(B.
   */
  final public static int OX_BYTE_BIG_INDIAN         = 0xff;

  /**
   * $B%^%7%sL>(B host $B$N%]!<%HHV9f(B Port $B$X(B TCP $B@\B3$r9T$J$$(B,
   * $B@\B3$r9T$J$&(B.
   */
  public OpenXMstream(String host,int Port) throws IOException{
    this(host,Port,false);
  }

  /**
   * $B%^%7%sL>(B host $B$N%]!<%HHV9f(B Port $B$X(B TCP $B@\B3$r9T$J$$(B,
   * $B@\B3$r9T$J$&(B. reverse $B$,(B true $B$N$H$-!"(B TCP $B%=%1%C%H$r(B
   * $B:n@.$7(B, $B@\B3$rBT$D(B.
   */
  public OpenXMstream(String host,int Port,boolean reverse) throws IOException{
    // create socket
    if(!reverse){
      socket = new Socket(host,Port);
    }else{
      socket = new ServerSocket(Port,50,InetAddress.getByName(host)).accept();
    }

    //is =new DebugInputStream(new BufferedInputStream(socket.getInputStream()));
    is = new BufferedInputStream(socket.getInputStream());
    os = socket.getOutputStream();
  }

  /**
   * $B%P%$%H%*!<%@$N7hDj$r9T$J$&(B. $B8=:_$O%M%C%H%o!<%/%P%$%H%*!<%@!<$N$_(B.
   */
  public int sendByteOrder() throws IOException{
    // send byte order
    os.write(OX_BYTE_NETWORK_BYTE_ORDER);
    os.flush();

    // receive byte order
    //System.err.println("get: "+is.read());
    is.read();

    order = OX_BYTE_NETWORK_BYTE_ORDER;

    return order;
  }

  /**
   * $B@\B3$rJD$8$k(B.
   */
  public void close() throws IOException{
    is.close();
    os.close();
    socket.close();
  }

  /**
   * 1 $B%P%$%H$NAw?.$r9T$J$&(B.
   */
  public void writeByte(int b) throws IOException{
    buffer.write(b);
  }

  /**
   * 32 bit integer $B$NAw?.$r9T$J$&(B.
   * $B$3$N%a%=%C%I$O(B, $B%P%$%H%*!<%@$NJQ49$r<+F0$G9T$J$&(B.
   */
  public void writeInt(int i) throws IOException{
    new DataOutputStream(buffer).writeInt(i);
  }

  /**
   * 1 $B%P%$%H$N<u?.$r9T$J$&(B.
   */
  public byte readByte() throws IOException{
    return (byte)is.read();
  }

  /**
   * 32 bit integer $B$N<u?.$r9T$J$&(B.
   * $B$3$N%a%=%C%I$O(B, $B%P%$%H%*!<%@$NJQ49$r<+F0$G9T$J$&(B.
   */
  public int readInt() throws IOException{
    return new DataInputStream(is).readInt();
  }

  /**
   * OpenXM $B%a%C%;!<%8$NAw?.$r9T$J$&(B.
   */
  public void write(OXmessage message) throws IOException,MathcapViolation{
    buffer.reset();
    message.write(this);
    buffer.writeTo(os);
    os.flush();
  }

  /**
   * OpenXM $B%a%C%;!<%8$NAw?.$r9T$J$&(B.
   * $B%\%G%#$N$_$G$h$/(B, $B%X%C%@$O<+F0$GIU2C$5$l$k(B.
   */
  public void send(OXbody object) throws IOException,MathcapViolation{
    this.write(new OXmessage(serial++,object));
  }

  /**
   * OXtag $B$KAjEv$9$k(B mathcap $B$rF@$k(B.
   * mathcap $B$,@_Dj$5$l$F$$$J$1$l$P(B, null $B$,JV$5$l$k(B.
   */
  public CMO[] getMathcap(int OXtag) throws IOException,MathcapViolation{
    if(mathcap != null){
      CMO[] list =((CMO_LIST)mathcap.getList().getElements()[2]).getElements();

      for(int i=0;i<list.length;i++){
	CMO[] datacap = ((CMO_LIST)list[i]).getElements();

	if(((CMO_INT32)datacap[0]).intValue() == OXtag){
	  return datacap;
	}
      }
    }

    return null;
  }

  /**
   * OX_SYNC_BALL $B$rAw?.$9$k(B. $B8=:_$OL$<BAu(B.
   */
  public void sendOX_SYNC_BALL() throws IOException,MathcapViolation{
    this.send(null);
  }

  /**
   * OpenXM $B%a%C%;!<%8$r<u?.$9$k(B.
   */
  public OXmessage receive() throws IOException{
    return new OXmessage(this);
  }

  /**
   * mathcap $B$r@_Dj$9$k(B.
   * $B0J8e(B, $BAw?.$9$k:]$K(B mathcap $B$KE,9g$7$F$$$k$+$I$&$+%A%'%C%/$,9T$J$o$l$k(B.
   */
  public void setMathCap(CMO_MATHCAP mathcap){
    this.mathcap = mathcap;
  }
}
