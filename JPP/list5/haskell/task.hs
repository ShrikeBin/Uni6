module Task where

import Data.List (nub)

-- 1. Binomial coefficient (recursive)
binomial :: Integer -> Integer -> Integer
binomial _ 0 = 1
binomial n k | n == k    = 1
             | otherwise = binomial (n-1) k + binomial (n-1) (k-1)

-- 2. Binomial using Pascal's triangle rows
pascalRow :: Integer -> [Integer]
pascalRow 0 = [1]
pascalRow n = zipWith (+) (0 : prev) (prev ++ [0])
  where prev = pascalRow (n-1)

binomial2 :: Integer -> Integer -> Integer
binomial2 n k = pascalRow n !! fromIntegral k

-- 3. Merge sort
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

-- 4. Diophantine equation: ax + by = gcd(a,b)
de :: Integer -> Integer -> (Integer, Integer, Integer)
de a 0 = (1, 0, a)
de a b =
  let (x', y', g) = de b (a `mod` b)
  in (y', x' - (a `div` b) * y', g)

-- 5. Prime factors
primeFactors :: Integer -> [Integer]
primeFactors n = go n 2
  where
    go 1 _ = []
    go m f
      | f * f > m      = [m]
      | m `mod` f == 0 = f : go (m `div` f) f
      | otherwise      = go m (f+1)

-- 6. Totient (classical)
totient :: Integer -> Integer
totient n = toInteger $ length [x | x <- [1..n], gcd x n == 1]

-- 7. Totient using prime factors
totient2 :: Integer -> Integer
totient2 n = n * product [p-1 | p <- uniquePrimes] `div` product uniquePrimes
  where uniquePrimes = nub (primeFactors n)

-- 8. Primes up to n (Sieve of Eratosthenes)
primes :: Integer -> [Integer]
primes n = sieve [2..n]
  where
    sieve []     = []
    sieve (p:xs) = p : sieve [x | x <- xs, x `mod` p /= 0]