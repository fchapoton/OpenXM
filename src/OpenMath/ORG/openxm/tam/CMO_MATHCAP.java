/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B MATHCAP $B7?$rI=$7$^$9(B.
 */
final public class CMO_MATHCAP extends CMO{
  private CMO_LIST list;

  /**
   * CMO_LIST src $B$rFbMF$H$9$k(B MATHCAP $B$r:n@.$7$^$9(B.
   * src $B$O(B MATHCAP $B$N%;%^%s%F%#%C%/%9$rK~$?$7$F$$$k$H2>Dj$7$F$$$^$9(B.
   */
  public CMO_MATHCAP(CMO_LIST src){
    list = src;
  }

  /**
   * $BFbMF$N(B LIST $B$rF@$^$9(B.
   */
  public CMO_LIST getList(){
    return list;
  }

  public int DISCRIMINATOR(){
    return CMO.MATHCAP;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    list.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_LIST list = (CMO_LIST)CMO.receive(is);

    return new CMO_MATHCAP(list);
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
