/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B STRING $B7?$rI=$7$^$9(B.
 */
final public class CMO_STRING extends CMO{
  private String str = null;

  /**
   * $B6uJ8;zNs$rFbMF$H$9$k(B STRING $B$r:n@.$7$^$9(B.
   */
  public CMO_STRING(){
    str = null;
  }

  /**
   * $BJ8;zNs(B a $B$rFbMF$H$9$k(B STRING $B$r:n@.$7$^$9(B.
   */
  public CMO_STRING(String a){
    str = a;
  }

  /**
   * $BFbMF$NJ8;zNs$rF@$^$9(B.
   */
  public String getString(){
    return str;
  }

  public int DISCRIMINATOR(){
    return CMO.STRING;
  }

  protected void sendByObject(OpenXMconnection os) throws IOException{
    byte[] buf = str.getBytes();

    os.writeInt(buf.length);
    for(int i=0;i<buf.length;i++){
      os.writeByte(buf[i]);
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    byte[] buf=null;

    len=is.readInt();
    if(len==0){
      return new CMO_STRING("");
    }

    buf = new byte[len];
    for(int i=0;i<len;i++){
      buf[i] = is.readByte();
    }

    return new CMO_STRING(new String(buf));
  }

  protected String toCMOexpressionByObject(){
    return "CMO_STRING,"+ str.length() +","+ str;
  }
}
