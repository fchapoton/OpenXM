/**
 * $OpenXM$
 */
package ORG.openxm.tam;

/**
 * OpenXM $B%5!<%P$N7W;;%W%m%;%9$r5-=R$9$k%/%i%9(B.
 */
public abstract class EngineProcess{
  private OpenXMstream stream;

  public EngineProcess(OpenXMstream stream){
    this.stream = stream;

    new Thread(){
      public void run(){
	readEvalLoop(getOpenXMstream());
      }
    }.start();
  }

  private OpenXMstream getOpenXMstream(){
    return stream;
  }

  abstract private void readEvalLoop(OpenXMstream DataStream);

  /**
   * main $B%a%=%C%I$N5-=RNc(B
   final public static void main(String[] argv) throws java.io.IOException{
     new EngineProcess(new OpenXMstream(System.in,System.out));
   }
   */
}
