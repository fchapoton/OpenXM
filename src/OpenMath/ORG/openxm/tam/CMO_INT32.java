/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B INT32 $B7?$rI=$7$^$9(B.
 */
final public class CMO_INT32 extends CMO{
  private int value;

  /**
   * i $B$rCM$H$9$k(B INT32 $B$r:n@.$7$^$9(B.
   */
  public CMO_INT32(int i){
    value = i;
  }

  public int DISCRIMINATOR(){
    return CMO.INT32;
  }

  /**
   * $BCM$rF@$^$9(B.
   */
  public int intValue(){
    return value;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
    os.writeInt(value);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int value = is.readInt();

    return new CMO_INT32(value);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
