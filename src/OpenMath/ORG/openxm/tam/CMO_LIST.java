/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_LIST.java,v 1.1 2000/09/12 07:05:06 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B LIST $B7?$rI=$7$^$9(B.
 */
final public class CMO_LIST extends CMO{
  private CMO ob[];

  /**
   * CMO $BG[Ns(B src $B$rMWAG$H$9$k(B LIST $B$r:n@.$7$^$9(B.
   */
  public CMO_LIST(CMO[] src){
    ob = src;
  }

  /**
   * $BMWAG$rF@$^$9(B.
   */
  public CMO[] getElements(){
    return ob;
  }

  public int DISCRIMINATOR(){
    return CMO.LIST;
  }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    os.writeInt(ob.length);

    for(int i=0;i<ob.length;i++){
      ob[i].write(os);
    }
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int len;
    CMO[] ob;

    len = is.readInt();
    ob = new CMO[len];

    for(int i=0;i<len;i++){
      ob[i] = CMO.receive(is);
    }

    return new CMO_LIST(ob);
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<ob.length;i++){
      //System.err.print(":"+ ob[i]);
      str += ","+ ob[i].toCMOexpression();
    }

    return "CMO_LIST,"+ ob.length + str;
  }
}
