/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_MONOMIAL32.java,v 1.2 2000/09/13 06:32:42 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.math.BigInteger;

/**
 * CMO $B7A<0$N(B MONOMIAL32 $B7?$rI=$7$^$9(B.
 * $B$3$N%/%i%9$OC19`<0$rI=8=$7$^$9(B.
 */
final public class CMO_MONOMIAL32 extends CMO{
  private int[] degree;
  private CMO coefficient;

  /**
   * $B78?t(B coefficient, $B<!?t(B degree[] $B$H$9$k(B MONOMIAL32 $B$r:n@.$7$^$9(B.
   */
  public CMO_MONOMIAL32(int[] degree,CMO coefficient){
    this.degree = degree;
    this.coefficient = coefficient;
  }

  public int DISCRIMINATOR(){
    return CMO.MONOMIAL32;
  }

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, DISCRIMINATOR()) 
			&& coefficient.allowQ(datacap);
    }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    os.writeInt(degree.length);
    for(int i=0;i<degree.length;i++){
      os.writeInt(degree[i]);
    }
    coefficient.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int n;
    int[] degree;
    CMO coefficient;

    n = is.readInt();
    degree = new int[n];

    for(int i=0;i<n;i++){
      degree[i] = is.readInt();
    }

    coefficient = CMO.receive(is);

    return new CMO_MONOMIAL32(degree,coefficient);
  }

  public String toCMOexpressionByObject(){
    String ret;

    ret = "CMO_MONOMIAL32,"+ degree.length;

    for(int i=0;i<degree.length;i++){
      ret += ","+ degree[i] ;
    }

    return ret +","+ coefficient.toCMOexpression();
  }

  /**
   * $B78?t$rF@$^$9(B.
   */
  public CMO getCoefficient(){
    return coefficient;
  }

  /**
   * $B<!?t$rF@$^$9(B.
   */
  public int[] getDegree(){
    return degree;
  }
}
