(* 1. Binomial coefficient — recursive *)
let rec binomial n k =
  if k = 0 || n = k then 1
  else binomial (n-1) k + binomial (n-1) (k-1)

(* 2. Binomial using Pascal's triangle rows *)
let next_row row =
  let shifted = 0 :: row in
  let paired  = row @ [0] in
  List.map2 (+) shifted paired

let pascal_row n =
  let rec go row i =
    if i = 0 then row
    else go (next_row row) (i-1)
  in go [1] n

let rec nth lst k =
  match lst, k with
  | x :: _, 0 -> x
  | _ :: t, k -> nth t (k-1)
  | [], _     -> failwith "index out of bounds"

let binomial2 n k = nth (pascal_row n) k

(* 3. Merge sort *)
let rec merge xs ys =
  match xs, ys with
  | [], ys -> ys
  | xs, [] -> xs
  | x :: xs', y :: ys' ->
    if x <= y then x :: merge xs' ys
    else            y :: merge xs  ys'

let split lst =
  let rec go l r = function
    | []      -> (l, r)
    | [x]     -> (x :: l, r)
    | x :: y :: rest -> go (x :: l) (y :: r) rest
  in go [] [] lst

let rec mergesort = function
  | []  -> []
  | [x] -> [x]
  | lst ->
    let (l, r) = split lst in
    merge (mergesort l) (mergesort r)

(* 4. Diophantine equation ax + by = gcd(a,b) *)
let rec de a b =
  if b = 0 then (1, 0, a)
  else
    let (x', y', g) = de b (a mod b) in
    (y', x' - (a / b) * y', g)

(* 5. Prime factors *)
let prime_factors n =
  let rec go m f acc =
    if m = 1 then List.rev acc
    else if f * f > m then List.rev (m :: acc)
    else if m mod f = 0 then go (m / f) f (f :: acc)
    else go m (f + 1) acc
  in go n 2 []

(* 6. Totient — classical *)
let rec gcd a b = if b = 0 then a else gcd b (a mod b)

let totient n =
  let rec go k acc =
    if k = 0 then acc
    else go (k-1) (if gcd k n = 1 then acc + 1 else acc)
  in go n 0

(* 7. Totient using prime_factors *)
let unique lst =
  List.fold_left (fun acc x -> if List.mem x acc then acc else x :: acc) [] lst

let totient2 n =
  if n = 1 then 1
  else
    let ps = unique (prime_factors n) in
    List.fold_left (fun acc p -> acc * (p - 1) / p) n ps

(* 8. Primes up to n — sieve *)
let primes n =
  let rec sieve = function
    | []      -> []
    | p :: xs -> p :: sieve (List.filter (fun x -> x mod p <> 0) xs)
  in
  let rec range a b = if a > b then [] else a :: range (a+1) b in
  sieve (range 2 n)