(*  
    SYNTAX

    ::  - add in front,
    @   - concat 
*)

fun binomial n 0 = 1
  | binomial n k = if n = k then 1
                   else binomial (n-1) k + binomial (n-1) (k-1)

fun nextRow row =
  let
    fun zipAdd []      []      = []
      | zipAdd (x::xs) []      = x :: zipAdd xs []
      | zipAdd []      (y::ys) = y :: zipAdd [] ys
      | zipAdd (x::xs) (y::ys) = (x+y) :: zipAdd xs ys
  in
    zipAdd (0 :: row) (row @ [0])
  end

fun pascalRow 0 = [1]
  | pascalRow n = nextRow (pascalRow (n-1))

fun nth (x::_)  0 = x
  | nth (_::xs) k = nth xs (k-1)
  | nth []      _ = raise Subscript

fun binomial2 n k = nth (pascalRow n) k

fun merge [] ys = ys
  | merge xs [] = xs
  | merge (x::xs) (y::ys) =
      if x <= y then x :: merge xs (y::ys)
      else            y :: merge (x::xs) ys

fun splitList [] = ([], [])
  | splitList [x] = ([x], [])
  | splitList (x::y::rest) =
      let val (l, r) = splitList rest
      in (x::l, y::r) 
    end

fun mergesort [] = []
  | mergesort [x] = [x]
  | mergesort lst =
      let val (l, r) = splitList lst
      in merge (mergesort l) (mergesort r)
    end

fun de a 0 = (1, 0, a)
  | de a b =
      let val (x', y', g) = de b (a mod b)
      in (y', x' - (a div b) * y', g) 
    end

fun primeFactors n =
  let
    fun go 1 _ = []
      | go m f =
          if f * f > m      then [m]
          else if m mod f = 0 then f :: go (m div f) f
          else go m (f+1)
  in go n 2 
  end

fun gcd a 0 = a
  | gcd a b = gcd b (a mod b)

fun totient n =
  let
    fun go 0 acc = acc
      | go k acc = go (k-1) (if gcd k n = 1 then acc+1 else acc)
  in go n 0
  end

fun nub [] = []
  | nub (x::xs) = x :: nub (List.filter (fn y => y <> x) xs)

fun totient2 1 = 1
  | totient2 n =
      let
        val ps = nub (primeFactors n)
        fun applyP acc p = acc * (p-1) div p
      in List.foldl ( fn (p, acc) => applyP acc p ) n ps 
    end

fun range a b = if a > b then [] else a :: range (a+1) b

fun sieve [] = []
  | sieve (p::xs) = p :: sieve (List.filter (fn x => x mod p <> 0) xs)

fun primes n = sieve (range 2 n)