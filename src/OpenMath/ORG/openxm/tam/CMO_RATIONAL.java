/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B RATIONAL $B7?$rI=$7$^$9(B.
 */
public class CMO_RATIONAL extends CMO{
  private CMO a,b;

  /**
   * (a/b) $B$rI=$9(B RATIONAL $B7?$rI=$7$^$9(B.
   * $B%;%^%s%F%#%C%/%9$N%A%'%C%/$O9T$J$o$l$^$;$s(B.
   */
  CMO_RATIONAL(CMO src_a,CMO src_b){
    a = src_a;
    b = src_b;
  }

  /**
   * $BJ,;R$rF@$^$9(B.
   */
  public CMO getBunshi(){
    return a;
  }

  /**
   * $BJ,Jl$rF@$^$9(B.
   */
  public CMO getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO.RATIONAL;
  }

  protected void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    a.write(os);
    b.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO a,b;

    a = CMO.receive(is);
    b = CMO.receive(is);

    return new CMO_RATIONAL(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_RATIONAL,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
