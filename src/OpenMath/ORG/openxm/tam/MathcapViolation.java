/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * OpenXM $B%a%C%;!<%8$rAw?.$9$k:]$KH/@8$9$kNc30(B Mathcap Violation $B$rI=$7$^$9(B.
 * $B$3$NNc30$O@_Dj$7$?(B mathcap $B$KH?$7$?%a%C%;!<%8$rAw?.$7$h$&$H$7$?(B
 * $B>l9g$K(B throw $B$5$l$^$9(B.
 */
public class MathcapViolation extends Exception{
  public MathcapViolation(){}

  public MathcapViolation(String s){
    super(s);
  }
}
