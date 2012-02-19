-- source code from Dbasic.m2
-- puts a module or matrix purely in shift degree 0.

zeroize2 = method()
zeroize2 Module := M -> (
     W := ring M;
     P := presentation M;
     coker map(W^(numgens target P), W^(numgens source P), P)
     )

zeroize2 Matrix := m -> (
     W := ring m;
     map(W^(numgens target m), W^(numgens source m), m)
     )

-- source code from Dresolution.m2
-- modified Dresolution
-- return hashTable{Dresolution,Transfermap}

shifts := (m, w, oldshifts) -> (
     tempmat := compress leadTerm m;
     if numgens source tempmat == 0 then newshifts := {}
     else (
     	  expmat := matrix(apply(toList(0..numgens source tempmat - 1), 
		    i -> (k := leadComponent tempmat_i;
	       	    	 append((exponents tempmat_(k,i))#0, oldshifts#k))));
     	  newshifts = (entries transpose (
	     	    expmat*(transpose matrix{ append(w, 1) })) )#0;
     	  );
     newshifts)

debug Core

kerGB := m -> (
     -- m should be a matrix which is a GB, and
     -- whose source has the Schreyer order.
     -- The resulting map will have the same form.
     map(ring m, rawKernelOfGB raw m)
     )

Dresolution2 = method( Options => {Strategy => Schreyer, LengthLimit => infinity} )
Dresolution2 Ideal := options -> I -> (
     Dresolution2((ring I)^1/I, options)
     )

Dresolution2 Module := options -> M -> (

     pInfo (1, "ENTERING Dresolution ... ");
     
     W := ring M;
     N := presentation M;

     pInfo (1, "Computing usual resolution using Schreyer order ..."); 
     pInfo (2, "��t Degree " | 0 | "...");
     pInfo (2, "��t��t��t Rank = " | rank target N | "��t time = 0. seconds");
     pInfo (2, "��t Degree " | 1 | "...");
     tInfo := toString first timing (m := schreyerOrder gens gb N);
     pInfo (2, "��t��t��t Rank = " | rank source N | "��t time = " |
	  tInfo | " seconds");

     M.cache.resolution = new ChainComplex;
     M.cache.resolution.ring = W;
     M.cache.resolution#0 = target m;
     M.cache.resolution#1 = source m;
     M.cache.resolution.dd#0 = map(W^0, target m, 0);
     M.cache.resolution.dd#1 = m;

     i := 2;
     while source m != 0 and i <= options.LengthLimit do (
	  pInfo (2, "��t Degree " | i | "...");
	  tInfo = toString first timing (m = kerGB m);
     	  M.cache.resolution#i = source m;
     	  M.cache.resolution.dd#i = m;
	  pInfo(2, "��t��t��t Rank = " | rank source m | "��t time = " |
	       tInfo | " seconds");
	  i = i+1;
	  );
     M.cache.resolution.length = i-1;
     M.cache.resolution
     )

Dresolution2 (Ideal, List) := options -> (I, w) -> (
     Dresolution2 ((ring I)^1/I, w, options)
     )

Dresolution2 (Module, List) := options -> (M, w) -> (

     pInfo (1, "ENTERING Dresolution ... ");
     
     -- ERROR CHECKING:
     W := ring M;
     k := options.LengthLimit;

     -- check that W is a Weyl algebra
     if W.monoid.Options.WeylAlgebra == {}
     then error "expected a Weyl algebra";
     if any(W.monoid.Options.WeylAlgebra, v -> class v =!= Option)
     then error "expected non-homogenized Weyl algebra";
     -- check that w is of the form (-u,u)
     createDpairs W;
     if #w =!= numgens W
     then error ("expected weight vector of length " | numgens W);
     if any( toList(0..#W.dpairInds#0 - 1),
	  i -> ( w#(W.dpairInds#0#i) + w#(W.dpairInds#1#i) != 0 ) )
     then error "expected weight vector of the form (-u,u)";

     -- PREPROCESSING
     if k == infinity then (
	  pInfo (1, "Computing adapted free resolution of length infinity using " 
	       | toString options.Strategy | " method...");
	  if (options.Strategy == Vhomogenize) then
     	  pInfo(2, "Warning: resolution via Vhomogenize might not terminate");
	  )
     else pInfo (1, "Computing adapted free resolution of length " | k | 
	  " using " | toString options.Strategy | " method...");

     homVar := symbol homVar;
     hvw := symbol hvw;
     if options.Strategy == Schreyer then (
    	  -- Make the homogenizing weight vector in HW
	  Hwt := toList(numgens W + 1:1);
     	  -- Make the V-filtration weight vector in HW
     	  Vwt := append(w,0);
     	  -- Make the homogeneous Weyl algebra
     	  HW := (coefficientRing W)(monoid [(entries vars W)#0, homVar,
	       WeylAlgebra => append(W.monoid.Options.WeylAlgebra, homVar),
	       MonomialOrder => {Weights=>Hwt, Weights=>Vwt, GRevLex}]);
     	  homVar = HW_homVar;
     	  WtoHW := map(HW, W, (vars HW)_{0..numgens W - 1});
     	  HWtoW := map(W, HW, (vars W)_{0..numgens W - 1} | matrix{{1_W}});
	  -- Also make the homogenizing Weyl algebra for shifts
     	  VW := (coefficientRing W)(monoid [hvw, (entries vars W)#0,
	       WeylAlgebra => W.monoid.Options.WeylAlgebra,
	       MonomialOrder => Eliminate 1]);
     	  HWtoVW := map(VW, HW, (vars VW)_{1..numgens W} | matrix{{VW_0}});
     	  VWtoHW := map(HW, VW, matrix{{homVar}} | (vars HW)_{0..numgens HW - 2});
	  hvwVar := VW_0;
	  HVWwt := prepend(-1,w);
	  VWwt := prepend(0,w);
	  )
     else if options.Strategy == Vhomogenize then (
     	  Hwt = prepend(-1,w);
     	  Vwt = prepend(0,w);
	  -- make the homogenizing Weyl algebra
     	  HW = (coefficientRing W)(monoid [homVar, (entries vars W)#0,
	       WeylAlgebra => W.monoid.Options.WeylAlgebra,
	       MonomialOrder => Eliminate 1]);
     	  homVar = HW_homVar;
     	  WtoHW = map(HW, W, (vars HW)_{1..numgens W});
     	  HWtoW = map(W, HW, matrix{{1_W}} | (vars W));
	  );

     -- CREATE AND INITIALIZE THE CHAIN COMPLEX
     --else 
     N := presentation M;
     --if (isSubmodule M) then N := presentation ((ambient M)/M);
     -- get the degree shifts right (need to check this against OT paper)
     if not M.cache.?resolution 
     then M.cache.resolution = new MutableHashTable;
     M.cache.resolution#w = new ChainComplex;
     M.cache.resolution#w.ring = W;
     s := rank source N;
     t := rank target N;
     M.cache.resolution#w#0 = target N;
     M.cache.resolution#w.dd#0 = map(W^0, M.cache.resolution#w#0, 0);

     -- MAKE THE FIRST STEP OF THE RESOLUTION
     shiftvec := apply(degrees target N, i -> i#0); 
     tempMap := map(HW^(-shiftvec), HW^(rank source N), WtoHW N);
     pInfo (2, "��t Degree 0...");
     pInfo (2, "��t��t��t Rank = " | t | "��t time = 0 seconds");
     pInfo (3, "��t Degree 1...");
     tInfo := toString first timing (
	  Jgb := gb(homogenize(tempMap, homVar, Hwt), ChangeMatrix => true);
      transfermap := getChangeMatrix Jgb;
      Jgb = homogenize(tempMap, homVar, Hwt)*transfermap;
	  if options.Strategy == Schreyer then Jgb = schreyerOrder Jgb;
	  if options.Strategy == Schreyer then (
	       tempMat := map(VW^(-shiftvec), VW^(numgens source Jgb), HWtoVW(Jgb));
	       shiftvec = shifts(homogenize(HWtoVW Jgb, hvwVar, HVWwt),
		    VWwt, shiftvec);
	       )
	  else shiftvec = shifts(Jgb, Vwt, shiftvec);
      transfermap = HWtoW transfermap;
	  M.cache.resolution#w#1 = W^(-shiftvec);
	  M.cache.resolution#w.dd#1 = map(M.cache.resolution#w#0, 
	       M.cache.resolution#w#1, HWtoW Jgb); 
	  );
     pInfo(2, "��t��t��t Rank = " | #shiftvec | "��t time = " |
	  tInfo | " seconds");
     startDeg := 2;
	  
     -- COMPUTE REST OF THE RESOLUTION
     i := startDeg;
     while i < k+1 and numgens source Jgb != 0 do (
	  pInfo (2, "��t Degree " | i | "...");
	  tInfo = toString first timing (
	       if options.Strategy == Schreyer then Jgb = kerGB Jgb
     	       else if options.Strategy == Vhomogenize then (
	     	    -- compute the kernel / syzygies
     	     	    Jsyz := syz Jgb;
	     	    -- put syzygies in the free module with the correct degree shifts
	     	    Jsyzmap := map(HW^(-shiftvec), HW^(numgens source Jsyz), Jsyz);
	     	    -- compute an adapted (-w,w)-GB of the syzygies module
     	     	    Jgb = gens gb homogenize(Jsyzmap, homVar, Hwt);
	     	    );
	       if options.Strategy == Schreyer then (
	       	    tempMat = map(VW^(-shiftvec), VW^(numgens source Jgb), HWtoVW(Jgb));
	       	    shiftvec = shifts(homogenize(tempMat, hvwVar, HVWwt), 
		       	 VWwt, shiftvec);
	       	    )
	       else shiftvec = shifts(Jgb, Vwt, shiftvec);
	       M.cache.resolution#w#i = W^(-shiftvec);
	       M.cache.resolution#w.dd#i = map(M.cache.resolution#w#(i-1),
	       	    M.cache.resolution#w#i, HWtoW Jgb);
	       );
	  pInfo(2, "��t��t��t Rank = " | #shiftvec | "��t time = " |
	       tInfo | " seconds");
	  i = i+1;
     	  );
     hashTable {VResolution =>M.cache.resolution#w, 
     TransferMap => transfermap}
     )


-- This routine computes a monomial from a list of variables 
-- and an exponent vector
List ^ List := (Vars, Exps) -> (
     product (Vars, Exps, (i,j) -> (i^j)) )

-- This routine takes a weight vector w of strictly positive integers
-- and returns the exponents beta in N^n such that 
-- ( k_0 < w_1 beta_1 + ... + w_n bet_n <= k_1) 
findExps := (w, k0, k1) -> (
     local minimum, local alpha, local tempExps;
     -- base case: weight vector w has dim 1
     if #w == 1 and k0 >= 0 then (
     	  tempExps = (toList((k0//w#0+1)..k1//w#0) / (i -> {i}) ) )
     else if #w == 1 and k0 < 0 then (
	  tempExps = ( toList(0..k1//w#0) / (i -> {i}) ) )
     else ( -- general case
	  tempExps = {};
	  alpha = 0;
	  while alpha <= k1//w#0 do (
	       tempExps = join( tempExps,
		    apply ( findExps( drop(w,1), k0-alpha*(w#0), 
			      k1-alpha*(w#0) ), j -> prepend(alpha,j) ) );
	       alpha = alpha+1;
	       );
	  );
     tempExps)


-- modified computeRestriction

computeRestriction2 = (M,wt,n0,n1,output,options) -> (

-- ERROR CHECKING
     W := ring M;
     createDpairs W;
     -- check weight vector
     if #wt != #W.dpairInds#0
     then error ("expected weight vector of length " | #W.dpairInds#0);
     if any(wt, i -> (i<0))
     then error "expected non-negative weight vector";

-- PREPROCESSING
     local tInfo;
     local tempvar;
     tempvar = symbol tempvar;
     tInfo = "";
     nW := numgens W;
     -- make the (-w,w) weight vector
     w := new MutableList from join(W.dpairInds#0,W.dpairInds#1);
     i := 0;
     while i < #W.dpairInds#0 do (
	  w#(W.dpairInds#0#i) = -wt#i;
	  w#(W.dpairInds#1#i) = wt#i; 
	  i = i+1;
	  );
     w = toList w;
     d := #positions(w, i->(i>0));
     -- the variables t_1, ..., t_d
     negVars := (entries vars W)#0_(positions(w, i->(i<0)));
     -- the substitution which sets t_1 = ... = t_d = 0
     resSub := apply( negVars, i -> (i => 0) );
     -- the variables Dt_1, ..., Dt_d, their indices, and their weights
     posVars := (entries vars W)#0_(positions(w, i->(i>0)));
     posInds := positions( w, i->(i>0) );
     posWeights := select( w, i->(i>0) );
     diffSub := apply( posVars, i -> (i => 0) );
     -- the rest of the variables x_1, ..., x_n, D_1, ..., D_n 
     otherVars := (entries vars W)#0_(positions(w, i->(i==0)));


-- MAKE THE WEYL ALGEBRA "resW" OF THE RESTRICTED SUBSPACE
     -- Case 1: if restriction to pt, then "resW" a field
     if #otherVars == 0 then (
	  resW := coefficientRing W;
	  WtoresW := map(resW, W, matrix{toList(numgens W: 0_resW)});
	  resWtoW := map(W, resW)
	  )
     -- Case 2: if restriction to coordinate subspace of dim d, then
     --         resW a Weyl algebra D_d.
     else (i = 0;
     	  resPairsList := {};
     	  while i < #otherVars do (
	       deriv := select(otherVars, j -> 
		    (j*otherVars#i - otherVars#i*j == 1));
	       if (#deriv == 1) then 
	       resPairsList = append(resPairsList, otherVars#i=>deriv#0);
	       i = i+1;
	       );
     	  resW = (coefficientRing W)(monoid [otherVars, WeylAlgebra=>resPairsList]);
     	  -- make the inclusion ring map "WtoresW" mapping W --> resW
     	  counter := 0;
     	  tempList := {};
	  WList := {};
	  i = 0;
     	  while i < numgens W do (
	       if w#i == 0 then (
	       	    tempList = append(tempList, resW_counter);
		    WList = append(WList, W_i);
	       	    counter = counter+1;)
	       else (tempList = append(tempList, 0_resW));
	       i = i+1;
	       );
     	  WtoresW = map(resW, W, matrix{tempList});
	  resWtoW = map(W, resW, matrix{WList});
	  );

-- INITIALIZE THE OUTPUT FORMS

     if member(HomologyModules, output) then homologyList := {};
     if member(Cycles, output) then kerList := {};
     if member(Boundaries, output) then imList := {};
     if member(GenCycles, output) then explicitList := {};
     if member(RestrictComplex,output) then (
	  restrictComplex := new ChainComplex;
	  restrictComplex.ring = resW;
	  );
     outputList := {};
     if member(Cycles, output) or member(Boundaries, output) or
     member(GenCycles, output) then explicitFlag := true 
     else explicitFlag = false;

-- GET MIN AND MAX ROOTS OF THE B-FUNCTION
     --<< "Computing b-function ";
     if n0 >= d then b := 1_(QQ[tempvar])
     else if rank ambient M == 1 then (
	  -- used to use "ideal presentation M" here
	  --<< "using ideal bFunction ";
	  pInfo(1, "Computing b-function using ideal bFunction... ");
     	  tInfo = toString first timing(
	       b = bFunction(ideal relations M, wt);
	       );
	  )
     else (
	  pInfo(1, "Computing b-function using module bFunction... ");
	  tInfo = toString first timing(
	       b = bFunctionM(M, wt, toList(rank ambient M: 0));
	       );
	  );
     
     if b == 0 then (
     	  error "Module not specializable. Restriction cannot be computed.";
	  );

     intRoots := getIntRoots b;
	  
-- NO INTEGER ROOTS
     if #intRoots == 0 then (
     	  k0 := 0;
     	  k1 := 0;
     	  pInfo(4, "��t bFunction = " | toString b);
     	  pInfo(2, "��t No integer roots");
     	  pInfo(3, "��t time = " | tInfo); 
	  if member(RestrictComplex, output) then (
	       restrictComplex#n0 = resW^0;
	       restrictComplex#n1 = resW^0;
	       restrictComplex.dd#n0 = map(resW^0,resW^0,0);
	       restrictComplex.dd#n1 = map(resW^0,resW^0,0);
	       i = n0+1;
	       while i < n1 do (
	       	    restrictComplex#i = resW^0;
	       	    restrictComplex.dd#i = map(resW^0,resW^0,0);
	       	    i = i+1;
	       	    );
	       );
	  if member(HomologyModules, output) then
     	  homologyList = apply (toList(n0+1..n1-1), i ->  (i => resW^0));
	  if member(Cycles, output) then
     	  kerList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));
	  if member(Boundaries, output) then
     	  imList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));
	  if member(GenCycles, output) then
     	  explicitList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));
	  )
     
-- INTEGER ROOTS EXIST
     else (
     k0 = min intRoots - 1;
     k1 = max intRoots;
     pInfo(4, "��t bFunction = " | toString b);
     pInfo(2, "��t min root =  " | k0+1 | " , max root =  " | k1);
     pInfo(3, "��t time = " | tInfo | " seconds");
     pInfo(2, " ");

----- SET k0 TO -infinity FOR EXPLICIT COHOMOLOGY CLASSES -----
     if member(Explicit, output) then k0 = -infinity;

-- COMPUTE FREE RESOLUTION ADAPTED TO THE WEIGHT VECTOR "w"
     tInfo = toString first timing (
     	  Resolution := Dresolution2 (M, w, LengthLimit => n1, Strategy => options.Strategy);
          C := Resolution#VResolution;
	  );
     pInfo(2, "��t Finished...");
     pInfo(2, "��t��t��t Total time = " | tInfo | " seconds");
     pInfo(2, " ");


-- COMPUTE THE RESTRICTED COMPLEX IN DEGREES "n0" TO "n1" 
     tInfo = toString first timing (
     pInfo(1, "Computing induced restriction complex in degrees " |
	  n0 | " to " | n1 | "...");

     -- INITIALIZE THE ITERATION : making the first differential
     pInfo(2, "��t Degree " | n0+1 | "...");

     -- MAKE THE TARGET MODULE AS DIRECT SUM OF D_m MODULES
     -- "targetGens" is a list of s lists, where the i-th list contains
     -- the monomial generators {dx^a} (as left D_m-module) of
     --      F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)
     tInfo = toString first timing (
     	  s := numgens target C.dd#(n0+1);
     	  targetDeg := degrees target C.dd#(n0+1);
     	  targetGens := {};
     	  if explicitFlag then targetMat := map(W^0,W^0,0);
     	  i = 0;
     	  while i < s do (
	       tempExps := findExps(posWeights, k0-targetDeg#i#0, k1-targetDeg#i#0);
	       tempGens := apply(tempExps, j -> posVars^j);
	       targetGens = append(targetGens, tempGens);
	       if explicitFlag then (
	       	    if tempGens == {} then (
		    	 targetMat = directSum(targetMat, compress matrix{{0_W}}); )
	       	    else (
		    	 targetMat = directSum(targetMat, matrix{tempGens}); );
	       	    );
	       i = i+1;	  
	       );
     	  targetSize := sum(targetGens, i->#i);  
          
	  -- MAKE THE SOURCE MODULE AS DIRECT SUM OF D_m MODULES
     	  -- "sourceGens" is a list of r lists, where the i-th list contains
     	  -- the monomial generators {dx^a} (as left D_m-module) of
     	  --    F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)
     	  m := C.dd#(n0+1);
     	  r := numgens C#(n0+1);
     	  sourceDeg := degrees C#(n0+1);
     	  sourceGens := {};
     	  if explicitFlag then sourceMat := map(W^0,W^0,0);
     	  i = 0;
     	  while i < r do (
	       -- Find generators of the current source
	       --    "F_k1(D_n/tD_n)^r/F_k0(D_n/tD_n)^r"
	       -- as a left D_m module.
	       -- They have the form { ��prod_{i=1}^n D_i^{a_i} }.
	       tempExps = findExps(posWeights, k0-sourceDeg#i#0, 
	       	    k1-sourceDeg#i#0);
	       tempGens = apply(tempExps, j -> posVars^j);
	       sourceGens = append(sourceGens, tempGens );
	       if explicitFlag then (
	       	    if tempGens == {} then (
		    	 sourceMat = directSum(sourceMat, compress matrix{{0_W}}); )
	       	    else (
		    	 sourceMat = directSum(sourceMat, matrix{tempGens}); );
	       	    );
	       i = i+1;
	       );
     	  sourceSize := sum(sourceGens, i -> #i);

	  
	  -- MAKE THE DIFFERENTIAL AS MATRIX OF D_m MODULES
     	  if sourceSize == 0 and targetSize == 0 then (
	       oldDiff := map(resW^0,resW^0,0) )
     	  else if sourceSize == 0 then ( oldDiff =
	       compress matrix toList(targetSize:{0_resW}) )
     	  else if targetSize == 0 then ( oldDiff =
	       transpose compress matrix toList(sourceSize:{0_resW}) )
     	  else (
	       -- For each generator j = ��prod_i D_i^{a_i}, compute its image
	       -- j*m_i as an element of the RHS. Get a matrix of image vectors.
	       imageMat := matrix join toSequence apply( r, a -> 
	       	    apply(sourceGens#a, b -> substitute(b*m_a, resSub) ) );
	       -- differentiate with respect to targetGens
	       oldDiff = transpose compress matrix toList(sourceSize:{0_W});
	       i = 0;
	       -- compute the induced image
	       while i < s do (
	       	    if targetGens#i =!= {} 
	       	    then oldDiff = oldDiff || substitute( 
		    	 contract(transpose matrix{targetGens#i}, imageMat^{i}), 
		    	 diffSub);
	       	    i = i+1;
	       	    );
	       oldDiff = WtoresW oldDiff;
	       );
     	  if member(RestrictComplex, output) then (
	       restrictComplex#n0 = resW^(rank target oldDiff);
	       restrictComplex#(n0+1) = resW^(rank source oldDiff);
	       restrictComplex.dd#(n0+1) = map(restrictComplex#n0,
	       	    restrictComplex#(n0+1), oldDiff);
	       );
     	  if member(Cycles, output) or member(Boundaries, output)
	  or member(GenCycles, output) then (
	       newKernel := zeroize2 mingens kernel oldDiff;
	       -- newKernel := zeroize2 gens kernel oldDiff;
	       explicitKernel := compress (sourceMat * resWtoW(newKernel));
	       );
	  );
     pInfo(2, "��t��t��t Rank = " | sourceSize | "��t time = " | tInfo | " seconds" );
     
     -- DO THE COMPUTATION IN HIGHER COHOMOLOGICAL DEGREES     
     s = r;
     targetGens = sourceGens;
     targetSize = sourceSize;
     if explicitFlag then targetMat = sourceMat;
     targetMat = sourceMat;
     currDeg := n0 + 2;
     --newKernel = 0;
     --explicitKernel = 0;
     while currDeg <= n1 and C#?(currDeg) do (
	  -- MAKE THE NEXT SOURCE MODULE
	  -- "sourceGens" is a list of r lists, where the i-th list contains
	  -- the monomial generators {dx^a} (as left D_m-module) of
	  --    F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)
	  pInfo(2, "��t Degree " | currDeg | "...");
	  tInfo = toString first timing (
	  r = numgens C#currDeg;
	  m = C.dd#currDeg;
	  sourceDeg = degrees C#(currDeg);
	  sourceGens = {};
	  if explicitFlag then sourceMat = map(W^0,W^0,0);
	  i = 0;
	  while i < r do (
	       -- Find generators of the current source
	       --    "F_k1(D_n/tD_n)^r/F_k0(D_n/tD_n)^r"
	       -- as a left D_m module.
	       -- They have the form { ��prod_{i=1}^n D_i^{a_i} }.
	       tempExps = findExps(posWeights, k0-sourceDeg#i#0, 
		    k1-sourceDeg#i#0);
	       tempGens = apply(tempExps, j -> posVars^j);
	       sourceGens = append(sourceGens, tempGens );
	       if explicitFlag then (
		    if tempGens == {} then (
 	       		 sourceMat = directSum(sourceMat, compress matrix{{0_W}}); )
		    else (
	       		 sourceMat = directSum(sourceMat, matrix{tempGens}); );
		    );
	       i = i+1;
	       );
	  sourceSize = sum(sourceGens, i -> #i);
	  
	  -- MAKE THE NEXT DIFFERENTIAL OF D_m MODULES	       
	  if sourceSize == 0 and targetSize == 0 then (
	       newDiff := map(resW^0,resW^0,0) )
	  else if sourceSize == 0 then ( newDiff =
	       compress matrix toList(targetSize:{0_resW}) )
	  else if targetSize == 0 then ( newDiff =
	       transpose compress matrix toList(sourceSize:{0_resW}) )
	  else (
	       -- For each generator j = ��prod_i D_i^{a_i}, compute its image
	       -- j*m_i as an element of the RHS. Get a matrix of image vectors.
	       imageMat = matrix join toSequence apply( r, a -> 
		    apply(sourceGens#a, b -> substitute(b*m_a, resSub) ) );
	       -- differentiate with respect to targetGens
	       newDiff = transpose compress matrix toList(sourceSize:{0_W});
	       i = 0;
	       while i < s do (
		    if targetGens#i =!= {}
		    then newDiff = newDiff || substitute( 
			 contract(transpose matrix{targetGens#i}, imageMat^{i}), 
			 diffSub);
		    i = i+1;
		    );
	       newDiff = WtoresW newDiff;
	       );
	  
	  -- UPDATE THE OUTPUT LIST
	  if member(RestrictComplex, output) then (
	       restrictComplex#currDeg = resW^(rank source newDiff);
	       restrictComplex.dd#currDeg = map(restrictComplex#(currDeg-1),
		    restrictComplex#currDeg, newDiff);
	       );
	  if member(HomologyModules, output) then (
	       tempHomology := homology(zeroize2 oldDiff, zeroize2 newDiff);
	       if tempHomology =!= null then
	       tempHomology = cokernel Dprune presentation tempHomology;
	       if tempHomology === null then tempHomology = resW^0;
	       homologyList = append(homologyList,
		    (currDeg-1) => tempHomology);
	       );
	  
	  -- MAKE EXPLICIT COHOMOLOGY CLASSES	       
     	  if member(Cycles, output) or member(Boundaries, output) or
	  member(GenCycles, output) then (
	       oldImage := zeroize2 mingens image newDiff;
	       -- oldImage := zeroize2 gens image newDiff;
	       if member(GenCycles, output) then (
	       	    if #otherVars == 0 then (
		    	 explicitList = append(explicitList,
			      (currDeg-1) => targetMat *
			      resWtoW(mingens subquotient(newKernel, oldImage))) )
	       	    else (
		    	 explicitList = append(explicitList,
			      (currDeg-1) => explicitKernel);
		    	 );
		    );
	       if member(Cycles, output) then 
	       kerList = append(kerList, (currDeg-1) => explicitKernel);
	       if member(Boundaries, output) then (
		    explicitImage := compress (targetMat * resWtoW(oldImage));
		    imList = append(imList, (currDeg-1) => explicitImage);
		    );
	       newKernel = zeroize2 mingens kernel newDiff;
	       -- newKernel = zeroize2 gens kernel newDiff;
	       explicitKernel = compress (sourceMat * resWtoW(newKernel));
	       );
	  
	  -- PREPARE FOR NEXT ITERATION
	  s = r;
	  targetGens = sourceGens;
	  targetSize = sourceSize;
	  if explicitFlag then targetMat = sourceMat;
	  oldDiff = newDiff;
	  currDeg = currDeg+1;
	  );
     	  pInfo(2, "��t��t��t Rank = " | targetSize | "��t time = " | tInfo | " seconds");
     	  );
     );
     pInfo(2, "��t Finished...");
     pInfo(2, "��t��t��t Total time = " | tInfo | " seconds");
     pInfo(2, " ");
  );

-- OUTPUT FORMAT
if member(HomologyModules, output) then outputList = append(
     outputList, HomologyModules => hashTable homologyList);
if member(GenCycles, output) then outputList = append(
     outputList, GenCycles => hashTable explicitList);
if member(Cycles, output) then outputList = append(
     outputList, Cycles => hashTable kerList);
if member(Boundaries, output) then outputList = append(
     outputList, Boundaries => hashTable imList);
if member(BFunction, output) then outputList = append(
     outputList, BFunction => factorBFunction b);
if member(VResolution, output) then outputList = append(
     outputList, VResolution => C);
if member(RestrictComplex, output) then outputList = append(
     outputList, RestrictComplex => restrictComplex);
if member(ResToOrigRing, output) then outputList = append(
     outputList, ResToOrigRing => resWtoW);
if member(TransferMap,output) then
     if Resolution =!= null then 
     outputList = append(outputList, TransferMap => Resolution#TransferMap);

hashTable outputList 
)


twistedLogCohomology2 = method( Options => {Strategy => Schreyer} );
twistedLogCohomology2 (List, List) := options -> (f, a) -> (
	k := #f;
	if k != #a
	then error "number of factors is not equal number of parametors";
	
	i := 0;
	prodf := 1;
	while i < k do (
		prodf = prodf * f_i;
		i = i+1;
	);
	
	R = ring prodf;
	if class R =!= PolynomialRing
	then error "expected element of a polynomial ring";
	if R.monoid.Options.WeylAlgebra =!= {}
	then error "expected element of a commutative polynomial ring";
	v := generators(R);
	n := #v;
	if n != 2
	then error "we compute 2-dimention logCohomology";
	d := {diff(v#1,prodf),-diff(v#0,prodf),prodf};
	syzf := kernel matrix{d};
	m := numgens syzf;
	msyz := gens syzf;
	W := makeWeylAlgebra( R, SetVariables=>false);
	phi := map(W,R);
	vw := generators(W);
	i = 0; ell := 0; j := 0, sum := 0;
	op := {};
	while i<m do (
		-- adjoint of differential operators
		j = 0; sum = 0;
		while j < k do (
			sum = sum + a_j * (msyz_(1,i) * diff(v#0,f_j) - msyz_(0,i) * diff(v#1,f_j)) // f_j;
			j = j+1;
		);
		ell = -phi msyz_(1,i) * vw_(2) + phi msyz_(0,i) * vw_(3) - phi msyz_(2,i) + phi sum;
	op = join(op,{ell});
	i = i+1;
	);
	fop := apply(op,Fourier);
	
	outputRequest := {GenCycles, HomologyModules, 
	VResolution, BFunction, Explicit, TransferMap};
	M := cokernel matrix{fop};
	outputTable := computeRestriction2(M, {1,1}, -1, n+1,
	outputRequest, options);
	outputList := {BFunction => outputTable#BFunction};
	if outputTable#VResolution =!= null then
	outputList = append(outputList,
	VResolution => FourierInverse outputTable#VResolution)
	else outputList = append(outputList, VResolution => null);
	
	outputList = outputList | {
	PreCycles => hashTable apply(toList(0..n),
		i -> (n-i => FourierInverse outputTable#GenCycles#i)),
	CohomologyGroups => hashTable apply(toList(0..n),
		i -> (n-i => outputTable#HomologyModules#i)) };
	
	intTable := hashTable outputList;
	
	-- make basis of H^0
	-- dim H^0 is -1 or 0
	basis0 := 0;
	if dim intTable#CohomologyGroups#0 == 0 then (
	sumx := 0; sumy := 0; j = 0;
	while j < k do (
		sumx = sumx + a_j * prodf * diff(v#0,f_j) // f_j;
		sumy = sumy + a_j * prodf * diff(v#1,f_j) // f_j;
		j = j+1;
	);
	I := ideal(phi prodf * vw_(2) + phi sumx, phi prodf * vw_(3) + phi sumy );
	basis0 = matrix{PolySols I};
	)
	else basis0 = intTable#PreCycles#0;
	
	-- make basis of H^1
	if outputTable#?TransferMap then(
	transfermap := transpose Dtransposition FourierInverse outputTable#TransferMap;
	
	mat :=  transpose(intTable#PreCycles#1)*transfermap;
	l := numRows mat;
	mat = mutableMatrix mat;
	i = 0;
	while i < l do(
		j = 0;
		while j < m do(
			mat_(i,j) = substitute(mat_(i,j),{vw_(2) => 0, vw_(3) => 0});
			j = j+1;
		);
	i = i+1;
	);
	mat = matrix mat;
	msyz = phi msyz;
	
	Basis1 := {};
	i = 0;
	while i < l do(
		base = 0; j = 0;
		while j < m do(
			base = base + mat_(i,j) * (msyz_(0,j) * vw_(2) + msyz_(1,j) * vw_(3));
			j = j+1;
		);
	Basis1 = join(Basis1,{base});
	i = i+1;
	);
	Basis1 = matrix{Basis1};
	)
	else Basis1 = intTable#PreCycles#1;
	
	if intTable#VResolution =!= null then (
		createDpairs W;
		Omega := Dres ideal W.dpairVars#1;
		Basis := hashTable {0 => basis0, 
		1 => Basis1,
		2 => intTable#PreCycles#2 * vw_(2)*vw_(3)};
		outputList = outputList | {LogBasis => 
		Basis, OmegaRes => Omega};
	);
	
	hashTable outputList
)

