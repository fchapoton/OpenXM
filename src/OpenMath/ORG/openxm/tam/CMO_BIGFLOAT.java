/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_BIGFLOAT.java,v 1.2 2000/09/13 06:32:42 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B BIGFLOAT $B7?$rI=$7$^$9(B.
 */
final public class CMO_BIGFLOAT extends CMO{
  // a * 2^e
  private CMO_ZZ a,e;

  /**
   * (a * 2^e) $B$rI=$9(B BIGFLOAT $B$r:n@.$7$^$9(B.
   */
  CMO_BIGFLOAT(CMO_ZZ a,CMO_ZZ e){
    this.a = a;
    this.e = e;
  }

  public int DISCRIMINATOR(){
    return CMO.BIGFLOAT;
  }

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, DISCRIMINATOR()) && a.allowQ(datacap);
    }

  public void sendByObject(OpenXMstream os) throws IOException{
    a.sendByObject(os);
    e.sendByObject(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    CMO_ZZ a,e;

    a = (CMO_ZZ) CMO_ZZ.receive(is);
    e = (CMO_ZZ) CMO_ZZ.receive(is);

    return new CMO_BIGFLOAT(a,e);
  }

  public String toCMOexpressionByObject(){
    return "CMO_BIGFLOAT,"+ a.toCMOexpression() +","+ e.toCMOexpression();
  }

  /**
   * $B>.?tIt$rF@$^$9(B.
   */
  public CMO_ZZ getSyosubu(){
    return a;
  }

  /**
   * $B;X?tIt$rF@$^$9(B.
   */
  public CMO_ZZ getShisubu(){
    return e;
  }
}
