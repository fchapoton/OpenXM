/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  CMO_QQ(){}

  CMO_QQ(CMO_ZZ src_a,CMO_ZZ src_b){
    a = src_a;
    b = src_b;
  }

  public CMO_ZZ getBunshi(){
    return a;
  }

  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO_QQ;
  }

  protected void sendByObject(DataOutputStream os) throws IOException{
    a.send(os);
    b.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a.receive(is);
    b.receive(is);

    return this;
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
