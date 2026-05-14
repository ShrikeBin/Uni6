module Main where

import Task

main :: IO ()
main = do
  putStrLn "=== binomial ==="
  print $ binomial 10 3   -- 120
  print $ binomial 6  3   -- 20
  print $ binomial 0  0   -- 1

  putStrLn "\n=== binomial2 (Pascal) ==="
  print $ binomial2 10 3  -- 120
  print $ binomial2 6  3  -- 20

  putStrLn "\n=== mergesort ==="
  print $ mergesort [5, 3, 8, 1, 9, 2, 7, 4, 6 :: Int]
  print $ mergesort ([] :: [Int])
  print $ mergesort [1 :: Int]

  putStrLn "\n=== de (Diophantine) ==="
  print $ de 30 20   -- gcd=10
  print $ de 35 15   -- gcd=5
  print $ de 7  3    -- gcd=1

  putStrLn "\n=== primeFactors ==="
  print $ primeFactors 360   -- [2,2,2,3,3,5]
  print $ primeFactors 13    -- [13]
  print $ primeFactors 1     -- []

  putStrLn "\n=== totient ==="
  print $ totient 36   -- 12
  print $ totient 1    -- 1
  print $ totient 7    -- 6

  putStrLn "\n=== totient2 ==="
  print $ totient2 36  -- 12
  print $ totient2 1   -- 1  (edge: no prime factors)
  print $ totient2 7   -- 6

  putStrLn "\n=== primes ==="
  print $ primes 50
  print $ primes 2
  print $ primes 1   -- []