/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B distributed polynomial $B7?$rI=$7$^$9(B.
 */
final public class CMO_DISTRIBUTED_POLYNOMIAL extends CMO{
  private CMO ring;
  private CMO_MONOMIAL32[] monomials;

  /**
   * $B4D(B ring $B>e$N(B MONOMIAL32 $B7?$NG[Ns(B monomials $B$r;}$DB?9`<0$r:n@.$7$^$9(B.
   */
  public CMO_DISTRIBUTED_POLYNOMIAL(CMO ring,CMO_MONOMIAL32[] monomials){
    this.ring = ring;
    this.monomials = monomials;
  }

  public int DISCRIMINATOR(){
    return CMO.DISTRIBUTED_POLYNOMIAL;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    if(monomials.length == 0){
      os.writeInt(1);
      ring.write(os);
      new CMO_ZERO().write(os);
    }else{
      os.writeInt(monomials.length);
      ring.write(os);
      for(int i=0;i<monomials.length;i++){
	monomials[i].write(os);
      }
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO ring;
    CMO_MONOMIAL32[] monomials;
    CMO tmp;
    int m;

    m = is.readInt();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_DMS_GENERIC)){
      System.err.println("not CMOobject");
    }
    ring = tmp;

    tmp = (CMO_MONOMIAL32)CMO.receive(is);
    if(tmp instanceof CMO_ZERO && m == 1){
      monomials = new CMO_MONOMIAL32[0];
    }else{
      monomials = new CMO_MONOMIAL32[m];
      monomials[0] = (CMO_MONOMIAL32)tmp;
    }

    for(int i=1;i<m;i++){
      monomials[i] = (CMO_MONOMIAL32)CMO.receive(is);
    }

    return new CMO_DISTRIBUTED_POLYNOMIAL(ring,monomials);
  }

  public String toCMOexpressionByObject(){
    String ret = "CMO_DISTRIBUTED_POLYNOMIAL,";

    if(monomials.length == 0){
      ret += 1 +",";
      ret += ring.toCMOexpression();
      ret += ","+ new CMO_ZERO().toCMOexpression();
    }else{
      ret += monomials.length +",";
      ret += ring.toCMOexpression();
      for(int i=0;i<monomials.length;i++){
	ret += ","+ monomials[i].toCMOexpression();
      }
    }

    return ret;
  }

  /**
   * $BB?9`<0$,Dj5A$5$l$F$$$k4D$rF@$^$9(B.
   */
  public CMO getRing(){
    return ring;
  }

  /**
   * $BB?9`<0$N3F(B monomial $B$rF@$^$9(B.
   */
  public CMO_MONOMIAL32[] getMonomials(){
    return monomials;
  }
}
