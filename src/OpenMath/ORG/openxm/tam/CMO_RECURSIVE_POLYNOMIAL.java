/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_RECURSIVE_POLYNOMIAL.java,v 1.2 2000/09/13 06:32:43 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B RECURSIVE POLYNOMIAL $B7?$rI=$7$^$9(B.
 */
final public class CMO_RECURSIVE_POLYNOMIAL extends CMO{
  private CMO_LIST variables;
  private CMO polynomial;

  /**
   * $BJQ?t(B variables $B$NB?9`<0(B polynomial $B$r:n@.$7$^$9(B.
   * $B%;%^%s%F%#%C%/%9$N%A%'%C%/$O9T$J$o$l$^$;$s(B.
   */
  public CMO_RECURSIVE_POLYNOMIAL(CMO_LIST variables,CMO polynomial){
    this.variables = variables;
    this.polynomial = polynomial;
  }

  /**
   * $BJQ?t$N%j%9%H$rF@$^$9(B.
   */
  public CMO_LIST getVariables(){
    return variables;
  }

  /**
   * $BB?9`<0$rF@$^$9(B.
   */
  public CMO getPolynomial(){
    return polynomial;
  }

  public int DISCRIMINATOR(){
    return CMO.RECURSIVE_POLYNOMIAL;
  }

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, DISCRIMINATOR()) 
			&& variables.allowQ(datacap)
			&& polynomial.allowQ(datacap);
    }

  protected void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    variables.write(os);
    polynomial.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    CMO_LIST variables;
    CMO polynomial;
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_LIST)){
      System.err.println("not CMOobject");
    }
    variables = (CMO_LIST)tmp;

    polynomial = CMO.receive(is);

    return new CMO_RECURSIVE_POLYNOMIAL(variables,polynomial);
  }

  protected String toCMOexpressionByObject(){
    String ret = "CMO_RECURSIVE_POLYNOMIAL";

    ret += ","+ variables.toCMOexpression();
    ret += ","+ polynomial.toCMOexpression();

    return ret;
  }
}
