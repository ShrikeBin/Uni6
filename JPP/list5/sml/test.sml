fun printList [] = print "[]\n"
  | printList lst =
      ( print "["
      ; List.app (fn x => print (Int.toString x ^ "; ")) (List.take (lst, length lst - 1))
      ; print (Int.toString (List.last lst))
      ; print "]\n" )

val () =
  ( print "=== binomial ===\n"
  ; print (Int.toString (binomial 10 3) ^ "\n")   (* 120 *)
  ; print (Int.toString (binomial 6  3) ^ "\n")   (* 20  *)
  ; print (Int.toString (binomial 0  0) ^ "\n")   (* 1   *)

  ; print "\n=== binomial2 (Pascal) ===\n"
  ; print (Int.toString (binomial2 10 3) ^ "\n")  (* 120 *)
  ; print (Int.toString (binomial2 6  3) ^ "\n")  (* 20  *)

  ; print "\n=== mergesort ===\n"
  ; printList (mergesort [5,3,8,1,9,2,7,4,6])
  ; printList (mergesort [])
  ; printList (mergesort [1])

  ; print "\n=== de (Diophantine) ===\n"
  ; let fun showDe a b =
          let val (x,y,z) = de a b
          in print ("de " ^ Int.toString a ^ " " ^ Int.toString b ^
                    " = (" ^ Int.toString x ^ ", " ^ Int.toString y ^
                    ", " ^ Int.toString z ^ ")\n") end
    in showDe 30 20; showDe 35 15; showDe 7 3 end

  ; print "\n=== primeFactors ===\n"
  ; printList (primeFactors 360)
  ; printList (primeFactors 13)
  ; printList (primeFactors 1)

  ; print "\n=== totient ===\n"
  ; print (Int.toString (totient 36) ^ "\n")   (* 12 *)
  ; print (Int.toString (totient 1)  ^ "\n")   (* 1  *)
  ; print (Int.toString (totient 7)  ^ "\n")   (* 6  *)

  ; print "\n=== totient2 ===\n"
  ; print (Int.toString (totient2 36) ^ "\n")  (* 12 *)
  ; print (Int.toString (totient2 1)  ^ "\n")  (* 1  *)
  ; print (Int.toString (totient2 7)  ^ "\n")  (* 6  *)

  ; print "\n=== primes ===\n"
  ; printList (primes 50)
  ; printList (primes 2)
  ; printList (primes 1)
  )