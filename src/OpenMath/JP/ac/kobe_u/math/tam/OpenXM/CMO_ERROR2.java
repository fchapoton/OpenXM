/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.java,v 1.8 2000/03/14 05:06:47 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_ERROR2 extends CMO{
  private CMO ob = new CMO_NULL();

  public CMO_ERROR2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO.ERROR2;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_ERROR2(CMO.receive(is));
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
