/*
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * OpenXM $B%a%C%;!<%8$N%\%G%#ItJ,$rI=$9Cj>]%/%i%9(B.
 */
public abstract class OXbody{

  //private protected OXbody(){}

  //public OXbody(DataInputStream is) throws IOException{}

  /**
   * os $B$X%*%V%8%'%/%H$rAw?.$7$^$9(B.
   * $B$3$N%a%=%C%I$G$O%X%C%@ItJ,$NAw?.$r5-=R$9$kI,MW$O$"$j$^$;$s(B.
   */
  public abstract void write(OpenXMstream os) throws IOException,MathcapViolation;

  //public abstract static OX read(DataInputStream is) throws IOException;

  /**
   * $B%*%V%8%'%/%H$r(B OXexpression $B$KJQ49$7$^$9(B.
   * $B$3$N%a%=%C%I$G$O%X%C%@ItJ,$NJQ49$r5-=R$9$kI,MW$O$"$j$^$;$s(B.
   */
  public abstract String toOXexpression();

  /**
   * $B%*%V%8%'%/%H$r(B String $B7?$KJQ49$7$^$9(B.
   * $B8=:_$O(B OXexpression $B$XJQ49$7$^$9(B.
   */
  public String toString(){
    return this.toOXexpression();
  }
}
