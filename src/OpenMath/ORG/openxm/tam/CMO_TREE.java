/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_TREE.java,v 1.1 2000/09/12 07:05:07 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO $B7A<0$N(B TREE $B7?$rI=$7$^$9(B.
 * $B$3$l$O(B OpenMath $B$N(B Content Dictionary $B$GDj5A$5$l$F$$$k%*%V%8%'%/%H$r(B
 * $BI=$9%/%i%9$G$9(B.
 */
final public class CMO_TREE extends CMO{
  private String name,cdname;
  private CMO_LIST leaves;

  /**
   * leaves $B$rMWAG$H$7(B cdname $B$GDj5A$5$l$F$$$k(B name $B$rI=$9(B TREE $B$r:n@.$7$^$9(B.
   * $B%;%^%s%F%#%C%/%9$O%A%'%C%/$5$l$^$;$s(B.
   */
  public CMO_TREE(String name,String cdname,CMO_LIST leaves){
    this.name = name;
    this.cdname = cdname;
    this.leaves = leaves;
  }

  /**
   * $BL>A0$rF@$^$9(B.
   */
  public String getName(){
    return name;
  }

  /**
   * $BL>A0$,Dj5A$5$l$F$$$k(B CD $B$NL>A0$rF@$^$9(B.
   */
  public String getCDName(){
    return cdname;
  }

  /**
   * $BMWAG$rF@$^$9(B.
   */
  public CMO_LIST getLeaves(){
    return leaves;
  }

  public int DISCRIMINATOR(){
    return CMO.TREE;
  }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    new CMO_STRING(name).write(os);
    new CMO_STRING(cdname).write(os);
    leaves.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    String name,cdname;
    CMO_LIST leaves;
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    name = ((CMO_STRING)tmp).getString();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    cdname = ((CMO_STRING)tmp).getString();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_LIST)){
      System.err.println("not CMOobject");
    }
    leaves = (CMO_LIST)tmp;

    return new CMO_TREE(name,cdname,leaves);
  }

  public String toCMOexpressionByObject(){
    return "CMO_TREE,"+ name +","+ cdname +","+ leaves.toCMOexpression();
  }
}
