/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B ERROR2 $B7?$rI=$7$^$9(B.
 */
final public class CMO_ERROR2 extends CMO{
  private CMO ob = new CMO_NULL();

  /**
   * CMO src $B$rFbMF$H$9$k(B ERROR2 $B$r:n@.$7$^$9(B.
   */
  public CMO_ERROR2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO.ERROR2;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_ERROR2(CMO.receive(is));
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
