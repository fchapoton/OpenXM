/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO.java,v 1.9 2000/02/21 03:48:21 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

abstract public class CMO extends OXbody{
  public static int[] mathcap = null;

  final public static int LARGEID     = 0x7f000000;
  final public static int CMO_ERROR   = ( LARGEID +1 );
  final public static int CMO_ERROR2  = ( LARGEID +2 );
  final public static int CMO_NULL    = 1;
  final public static int CMO_INT32   = 2;
  final public static int CMO_DATUM   = 3;
  final public static int CMO_STRING  = 4;
  final public static int CMO_MATHCAP = 5;
  final public static int CMO_LIST    = 17;

  final public static int CMO_MONOMIAL32         = 19;
  final public static int CMO_ZZ                 = 20;
  final public static int CMO_QQ                 = 21;
  final public static int CMO_ZERO               = 22;
  final public static int CMO_DMS                = 23;
  final public static int CMO_DMS_GENERIC        = 24;
  final public static int CMO_DMS_OF_N_VARIABLES = 25;
  final public static int CMO_RING_BY_NAME       = 26;

  final public static int CMO_RECURSIVE_POLYNOMIAL       = 27;
  final public static int CMO_DISTRIBUTED_POLYNOMIAL     = 31;
  final public static int CMO_POLYNOMIAL_IN_ONE_VARIABLE = 33;

  final public static int CMO_RATIONAL                   = 34;

  final public static int CMO_64BIT_MACHINE_DOUBLE           = 40;
  final public static int CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE  = 41;
  final public static int CMO_128BIT_MACHINE_DOUBLE          = 42;
  final public static int CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE = 43;

  final public static int CMO_BIGFLOAT          = 50;
  final public static int CMO_IEEE_DOUBLE_FLOAT = 51;

  final public static int CMO_INDETERMINATE      = 60;
  final public static int CMO_TREE               = 61;
  final public static int CMO_LAMBDA             = 62;

  final public static int CMO_PRIVATE = 0x7fff0000;

  public CMO(){}

  public CMO(InputStream is) throws IOException{
    //DataInputStream is;
  }

  abstract protected int DISCRIMINATOR();

  public int getDISCRIMINATOR(){
    return this.DISCRIMINATOR();
  }

  abstract protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation;

  final public void write(OpenXMconnection os)
       throws IOException,MathcapViolation{
    write(new DataOutputStream(os.os));
  }

  final protected void write(DataOutputStream os)
       throws IOException,MathcapViolation{
    if(mathcap != null){ // check mathcap
      int i=0;

      for(;i<mathcap.length;i++){
	if(mathcap[i] == this.DISCRIMINATOR()){
	  break;
	}
      }
      if(i>=mathcap.length){
	throw new MathcapViolation(this.toCMOexpression());
      }
    }
    os.writeInt(this.DISCRIMINATOR());
    this.sendByObject(os);
  }

  public static CMO receive(DataInputStream is) throws IOException{
    int a = 0;

    a = is.readInt();

    switch(a){
    case CMO_ERROR:
      return new CMO_ERROR().receiveByObject(is);

    case CMO_ERROR2:
      return cmoError2.receive(is);

    case CMO_NULL:
      return cmoNull.receive(is);

    case CMO_INT32:
      return new CMO_INT32().receiveByObject(is);

    case CMO_DATUM:
      return new CMO_DATUM().receiveByObject(is);

    case CMO_STRING:
      return new CMO_STRING().receiveByObject(is);

    case CMO_MATHCAP:
      return new CMO_MATHCAP().receiveByObject(is);

    case CMO_LIST:
      return new CMO_LIST().receiveByObject(is);

    case CMO_MONOMIAL32:
      return new CMO_MONOMIAL32().receiveByObject(is);

    case CMO_ZZ:
      return new CMO_ZZ().receiveByObject(is);

    case CMO_QQ:
      return new CMO_QQ().receiveByObject(is);

    case CMO_ZERO:
      return new CMO_ZERO().receiveByObject(is);

    case CMO_DMS:
      return new CMO_DMS().receiveByObject(is);

    case CMO_DMS_GENERIC:
      return new CMO_DMS_GENERIC().receiveByObject(is);

    case CMO_RECURSIVE_POLYNOMIAL:
      return new CMO_RECURSIVE_POLYNOMIAL().receiveByObject(is);

    case CMO_DISTRIBUTED_POLYNOMIAL:
      return new CMO_DISTRIBUTED_POLYNOMIAL().receiveByObject(is);

    case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
      return new CMO_POLYNOMIAL_IN_ONE_VARIABLE().receiveByObject(is);

    case CMO_RATIONAL:
      return new CMO_RATIONAL().receiveByObject(is);

    case CMO_BIGFLOAT:
      return new CMO_BIGFLOAT().receiveByObject(is);

    case CMO_INDETERMINATE:
      return new CMO_INDETERMINATE().receiveByObject(is);

    case CMO_TREE:
      return new CMO_TREE().receiveByObject(is);

    default:
      System.err.println("unknown CMO type("+ a +")");
    }
    return null;
  }

  abstract protected String toCMOexpressionByObject();

  final public String toCMOexpression(){
    return "("+ this.toCMOexpressionByObject() +")";
  }

  final public String toOXexpression(){
    return toCMOexpression();
  }

  final public String toString(){
    return this.toCMOexpression();
  }
}
