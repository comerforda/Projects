(*
    Adam Comerford (comer059)
    csci_2041 section 001
    10/22/2019
    Project 1 - tautology checker
    
    This program receives a propositional logic expression and tests to see if it is a tautology.
    It does so by first turning any expression into a Conditional (If statement) through equivalence
    rules, then simplifies the conditional using simplification rules. If the expression is a tautology,
    program returns 'true'. If it is not, returns 'false'.
    This project exhibits proficiency of the functional programming style, heavily involving and pattern matching
    and recursive techniques.
*)

type proposition = 
    False |
    True |
    Var of string |
    And of proposition * proposition |
    Or of proposition * proposition |
    Not of proposition |
    Imply of proposition * proposition |
    Equiv of proposition * proposition ;;

type conditional = 
    IffyFalse |
    IffyTrue |
    IffyVar of string |
    If of conditional * conditional * conditional ;;

(* Turn any expression into a conditional. Recursively call on the parts
  in case they are also expressions. *)
let rec ifify p =
    match p with
        False -> IffyFalse |
        True -> IffyTrue |
        Var a -> IffyVar a |
        Not a -> If (ifify a, IffyFalse, IffyTrue) |
        And (a, b) -> If(ifify a, ifify b, IffyFalse) |
        Or (a, b) -> If (ifify a, IffyTrue, ifify b) |
        Imply (a, b) -> If (ifify a, ifify b, IffyTrue) |
        Equiv (a, b) -> If(ifify a, ifify b, If(ifify b, IffyFalse, IffyTrue))
;;

(* c is an 'If' conditional, must normalize it: *)
let rec normalize c =
    match c with
        IffyFalse -> IffyFalse |
        IffyTrue -> IffyTrue |
        IffyVar s -> IffyVar s |
        If (test, a2, b2) -> 
            match test with
                IffyVar pi -> If (IffyVar pi, normalize a2, normalize b2) |
                IffyTrue -> If (test, normalize a2, normalize b2) |
                IffyFalse -> If (test, normalize a2, normalize b2) |
                If (pi, a1, b1) -> 
                    normalize (If(pi, (If (a1, a2, b2)), (If (b1, a2, b2))))
;;

(*  Replace any occurance of v, in c, with b *)
let rec substitute c v b =
    match c with
        IffyTrue -> c |
        IffyFalse -> c |
        IffyVar s ->    (* Replaces our normalized pi Vars *)
            if (IffyVar s = v)
            then b 
            else c |
        If (pi, a, b0) ->
            If(substitute pi v b, substitute a v b, substitute b0 v b)
;;

(*  Rule 11: (IF π α β) ⇒ (IF π α{π ⇒ true} β{π ⇒ false}) *)
let rec simplify c =
    match c with
        If (pi, a, b) ->
        (*  Here be Rule 11:
            break parts into new names to be worked with.
            Based on hints from lecture *)
            let a = simplify (substitute a pi IffyTrue)
            in let b = simplify (substitute b pi IffyFalse)
            in
            if pi = IffyTrue then a       (* Rule 7 *)
            else if pi = IffyFalse then b (* Rule 8 *)
            else if ((a = IffyTrue) && (b = IffyFalse)) then pi (* Rule 9 *)
            else if a = b then a    (* Budget Rule 10 *)
            else If (pi, a, b) |
        _ -> c  (*If it's anything else, nothing to simplify - return c *)
;;

let tautology p =
    let c = ifify p
    in let c' = normalize c
    in let bool = simplify c'
    in 
    if bool = IffyTrue
        then true
    else
        false
;;

(*  DeMorgans:
    (Imply (Not(And(Var "p", Var "q")), Or(Not (Var "p"), Not(Var "q"))))
*)
