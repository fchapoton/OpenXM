/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_NULL.java,v 1.1 2000/09/12 07:05:06 tam Exp $
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

  public void sendByObject(OpenXMstream os) throws IOException{
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
