/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_DATUM.java,v 1.1 2000/09/12 07:05:05 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B DATUM $B7?$rI=$7$^$9(B.
 */
public class CMO_DATUM extends CMO{
  private byte data[];

  /**
   * in[] $B$rFbMF$H$9$k(B DATUM $B7?$r:n@.$7$^$9(B.
   */
  CMO_DATUM(byte in[]){
    data = in;
  }

  public int DISCRIMINATOR(){
    return DATUM;
  }

  public void sendByObject(OpenXMstream os) throws IOException{
    os.writeInt(data.length);
    for(int i=0;i<data.length;i++){
      os.writeByte(data[i]);
    }
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int len;
    byte[] data;

    len = is.readInt();
    data = new byte[len];

    for(int i=0;i<len;i++){
      data[i] = is.readByte();
    }

    return new CMO_DATUM(data);
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<data.length;i++){
      str += ","+ data[i];
    }

    return "CMO_DATUM,"+ data.length + str;
  }
}
