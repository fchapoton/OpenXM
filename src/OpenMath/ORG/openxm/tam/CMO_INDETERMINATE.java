/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_INDETERMINATE.java,v 1.1 2000/09/12 07:05:06 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B INDETERMINATE $B7?$rI=$7$^$9(B.
 */
final public class CMO_INDETERMINATE extends CMO{
  private CMO_STRING variable;

  /**
   * $BJQ?tL>(B str $B$H$J$k(B INDETERMINATE $B$r:n@.$7$^$9(B.
   */
  public CMO_INDETERMINATE(String str){
    variable = new CMO_STRING(str);
  }

  /**
   * $BJQ?tL>$rF@$^$9(B.
   */
  public String getString(){
    return variable.getString();
  }

  public int DISCRIMINATOR(){
    return CMO.INDETERMINATE;
  }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    variable.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    CMO_STRING variable;
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    variable = (CMO_STRING)tmp;

    return new CMO_INDETERMINATE(variable.getString());
  }

  public String toCMOexpressionByObject(){
    return "CMO_INDETERMINATE,"+ variable.toCMOexpression();
  }
}
