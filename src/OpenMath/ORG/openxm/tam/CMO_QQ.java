/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;
import java.math.BigInteger;

/**
 * CMO $B7A<0$N(B QQ $B7?$rI=$7$^$9(B.
 */
final public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  /**
   * (a/b) $B$rI=$9(B QQ $B7?$rI=$7$^$9(B.
   * $B%;%^%s%F%#%/%9$N%A%'%C%/$O9T$J$o$l$^$;$s(B.
   */
  public CMO_QQ(CMO_ZZ a,CMO_ZZ b){
    this.a = a;
    this.b = b;
  }

  /**
   * $BJ,;R$rF@$^$9(B.
   */
  public CMO_ZZ getBunshi(){
    return a;
  }

  /**
   * $BJ,Jl$rF@$^$9(B.
   */
  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO.QQ;
  }

  private int sign(int a){
    if(a>0){
      return 1;
    }else if(a<0){
      return -1;
    }
    return 0;
  }

  protected void sendByObject(OpenXMconnection os) throws IOException{
    a.sendByObject(os);
    b.sendByObject(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_ZZ a,b;

    a = (CMO_ZZ)CMO_ZZ.receive(is);
    b = (CMO_ZZ)CMO_ZZ.receive(is);

    return new CMO_QQ(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
