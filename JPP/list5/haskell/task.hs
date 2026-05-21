module Task where

import Data.List (nub)

binomial :: Integer -> Integer -> Integer
binomial _ 0 = 1
binomial n k | n == k    = 1
             | otherwise = binomial (n-1) k + binomial (n-1) (k-1)

pascalRow :: Integer -> [Integer]
pascalRow 0 = [1]
pascalRow n = zipWith (+) (0 : prev) (prev ++ [0])
  where prev = pascalRow (n-1)

-- !! (list index)
binomial2 :: Integer -> Integer -> Integer
binomial2 n k = pascalRow n !! fromIntegral k 

mergesort :: Ord a => [a] -> [a]
mergesort []  = []
mergesort [x] = [x]
mergesort xs  = merge (mergesort left) (mergesort right)
  where
    mid   = length xs `div` 2
    left  = take mid xs
    right = drop mid xs

merge :: Ord a => [a] -> [a] -> [a]
merge [] ys = ys
merge xs [] = xs
merge (x:xs) (y:ys)
  | x <= y    = x : merge xs (y:ys)
  | otherwise = y : merge (x:xs) ys

de :: Integer -> Integer -> (Integer, Integer, Integer)
de a 0 = (1, 0, a)
de a b =
  let (x', y', g) = de b (mod a b)
  in (y', x' - (div a b) * y', g)

primeFactors :: Integer -> [Integer]
primeFactors n = go n 2
  where
    go 1 _ = []
    go m f
      | f * f > m      = [m]
      | m `mod` f == 0 = f : go (m `div` f) f
      | otherwise      = go m (f+1)

totient :: Integer -> Integer
totient n = toInteger $ length [x | x <- [1..n], gcd x n == 1]

totient2 :: Integer -> Integer
totient2 n = div (n * product [p - 1 | p <- uniquePrimes]) (product uniquePrimes)
  where uniquePrimes = nub (primeFactors n)

primes :: Integer -> [Integer]
primes n = sieve [2..n]
  where
    sieve []     = []
    sieve (p:xs) = p : sieve [x | x <- xs, mod x p /= 0]