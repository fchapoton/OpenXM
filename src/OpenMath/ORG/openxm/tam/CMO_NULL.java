/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B NULL $B7?$rI=$7$^$9(B.
 */
final public class CMO_NULL extends CMO{
  /**
   * NULL $B$r:n@.$7$^$9(B.
   */
  public CMO_NULL(){}

  public int DISCRIMINATOR(){
    return CMO.NULL;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
