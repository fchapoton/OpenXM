/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B DMS $B7?$rI=$7$^$9(B.
 */
final public class CMO_DMS extends CMO{
  /**
   * DMS $B$r:n@.$7$^$9(B.
   */
  public CMO_DMS(){}

  public int DISCRIMINATOR(){
    return CMO.DMS;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_DMS();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
