/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO.java,v 1.1 2000/09/09 11:54:08 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * OpenXM $B%a%C%;!<%8$N(B CMO $B%*%V%8%'%/%H$rI=$9Cj>]%/%i%9(B.
 */
abstract public class CMO extends OXbody{
  final public static int LARGEID     = 0x7f000000;
  final public static int ERROR   = ( LARGEID +1 );
  final public static int ERROR2  = ( LARGEID +2 );
  final public static int NULL    = 1;
  final public static int INT32   = 2;
  final public static int DATUM   = 3;
  final public static int STRING  = 4;
  final public static int MATHCAP = 5;
  final public static int LIST    = 17;

  final public static int MONOMIAL32         = 19;
  final public static int ZZ                 = 20;
  final public static int QQ                 = 21;
  final public static int ZERO               = 22;
  final public static int DMS                = 23;
  final public static int DMS_GENERIC        = 24;
  final public static int DMS_OF_N_VARIABLES = 25;
  final public static int RING_BY_NAME       = 26;

  final public static int RECURSIVE_POLYNOMIAL       = 27;
  final public static int DISTRIBUTED_POLYNOMIAL     = 31;
  final public static int POLYNOMIAL_IN_ONE_VARIABLE = 33;

  final public static int RATIONAL                   = 34;

  final public static int CMO_64BIT_MACHINE_DOUBLE       = 40;
  final public static int ARRAY_OF_64BIT_MACHINE_DOUBLE  = 41;
  final public static int CMO_128BIT_MACHINE_DOUBLE      = 42;
  final public static int ARRAY_OF_128BIT_MACHINE_DOUBLE = 43;

  final public static int BIGFLOAT          = 50;
  final public static int IEEE_DOUBLE_FLOAT = 51;

  final public static int INDETERMINATE      = 60;
  final public static int TREE               = 61;
  final public static int LAMBDA             = 62;

  final public static int PRIVATE = 0x7fff0000;

  abstract protected int DISCRIMINATOR();

  public int getDISCRIMINATOR(){
    return this.DISCRIMINATOR();
  }

  abstract protected void sendByObject(OpenXMstream stream)
       throws IOException,MathcapViolation;

  final public void write(OpenXMstream os)
       throws IOException,MathcapViolation{
    CMO[] mathcap = os.getMathcap(OXmessage.OX_DATA);

    if(mathcap != null){ // check mathcap
      CMO[] datacap = ((CMO_LIST)mathcap[1]).getElements();
      int i=0;

      for(;i<datacap.length;i++){
	if(((CMO_INT32)datacap[i]).intValue() == this.DISCRIMINATOR()){
	  break;
	}
      }
      if(i>=datacap.length){
	throw new MathcapViolation(this.toCMOexpression());
      }
    }
    os.writeInt(this.DISCRIMINATOR());
    this.sendByObject(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int a = is.readInt();

    switch(a){
    case CMO.ERROR2:
      return CMO_ERROR2.receive(is);

    case CMO.NULL:
      return CMO_NULL.receive(is);

    case CMO.INT32:
      return CMO_INT32.receive(is);

    case CMO.DATUM:
      return CMO_DATUM.receive(is);

    case CMO.STRING:
      return CMO_STRING.receive(is);

    case CMO.MATHCAP:
      return CMO_MATHCAP.receive(is);

    case CMO.LIST:
      return CMO_LIST.receive(is);

    case CMO.MONOMIAL32:
      return CMO_MONOMIAL32.receive(is);

    case CMO.ZZ:
      return CMO_ZZ.receive(is);

    case CMO.QQ:
      return CMO_QQ.receive(is);

    case CMO.ZERO:
      return CMO_ZERO.receive(is);

    case CMO.DMS:
      return CMO_DMS.receive(is);

    case CMO.DMS_GENERIC:
      return CMO_DMS_GENERIC.receive(is);

    case CMO.RECURSIVE_POLYNOMIAL:
      return CMO_RECURSIVE_POLYNOMIAL.receive(is);

    case CMO.DISTRIBUTED_POLYNOMIAL:
      return CMO_DISTRIBUTED_POLYNOMIAL.receive(is);

    case CMO.POLYNOMIAL_IN_ONE_VARIABLE:
      return CMO_POLYNOMIAL_IN_ONE_VARIABLE.receive(is);

    case CMO.RATIONAL:
      return CMO_RATIONAL.receive(is);

    case CMO.BIGFLOAT:
      return CMO_BIGFLOAT.receive(is);

    case CMO.INDETERMINATE:
      return CMO_INDETERMINATE.receive(is);

    case CMO.TREE:
      return CMO_TREE.receive(is);

    default:
      System.err.println("unknown CMO type("+ a +")");
    }
    return null;
  }

  abstract protected String toCMOexpressionByObject();

  /**
   * $B%*%V%8%'%/%H$r(B CMO expression $B$KJQ49$7$^$9(B.
   */
  final public String toCMOexpression(){
    return "("+ this.toCMOexpressionByObject() +")";
  }

  /**
   * $B%*%V%8%'%/%H$r(B OX expression $B$KJQ49$7$^$9(B.
   * $B8=:_$O(B CMO expression $B$X$HJQ49$7$^$9(B.
   */
  final public String toOXexpression(){
    return toCMOexpression();
  }

  final public String toString(){
    return this.toCMOexpression();
  }
}
