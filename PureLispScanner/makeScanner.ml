(*

  Token scanner for the Pure Lisp interpreter.

    Adam Comerford (comer059)
    24 Nov 19

*)
exception ErrorIn of string;;

type token =
  CloseParenToken |
  EndToken |
  NumberToken of int |
  OpenParenToken |
  SymbolToken of string ;;

(* MAKE SCANNER. Return a function NEXT TOKEN that reads TOKENs from a file
   with pathname PATH. OCaml RAISEs an exception if there's no such file. *)

let makeScanner path =

  let input = (open_in path)  (* INPUT. Read chars from this channel. *)
  in let ch = ref ' '         (* CH. The char most recently read from INPUT. *)
  in
(* NEXT CHAR. Advance CH to the next char from INPUT, or to '\000' if we're at
   the end of INPUT. *)
  let nextChar () =
    try ch := input_char input
    with End_of_file -> ch := '\000'
  in 
    (* Begin helpers *) 
    let nextEndToken () = 
      EndToken

    in let nextOpenParenToken () = 
       nextChar (); 
       OpenParenToken

    in let nextCloseParenToken () =
       nextChar (); 
       CloseParenToken

    in let rec nextNumberToken prefix = 
       let numString = prefix ^ (Char.escaped !ch)
       in nextChar ();
          match !ch with
          | '0' .. '9' -> nextNumberToken numString (* nextChar is a number, add to our string *)
          | _ -> let n = int_of_string (numString)  (* nextChar is not a number, return our int *)
                 in NumberToken n

    in let rec nextSymbolToken prefix = 
       let string = prefix ^ (Char.escaped !ch)
       in nextChar ();
          match !ch with
          | 'a' .. 'z' -> nextSymbolToken string (* nextChar is a letter, add to the string. *)
          | 'A' .. 'Z' -> nextSymbolToken string
          | _ -> SymbolToken string              (* nextChar is not a letter, return our string *)

    in let rec nextNumberOrSymbolToken () = 
       nextChar ();
       match !ch with
       | '0' .. '9' -> nextNumberToken "-" (* NexChar is a digit, get rest of number *)
       | 'a' .. 'z' -> nextSymbolToken "-" (* NextChar is a symbol, get rest of the string *)
       | 'A' .. 'Z' -> nextSymbolToken "-" (* NextChar is a symbol, get rest of the string *)
       | '\ ' -> SymbolToken "-" (* "-" is isolated, and therefore should be printed as a symbol*)
       | '\n' -> SymbolToken "-" (* "-" is isolated, and therefore should be printed as a symbol*)
       | _ -> raise (ErrorIn "nextNumberOrSymbolToken") (* This shouldn't happen *)
  in
  (* DISPATCHER *)
  let rec nextToken () = 
    match !ch with
    | '\000' -> nextEndToken ()
    | '\ ' -> nextChar (); nextToken ()
    | '\n' -> nextChar (); nextToken ()
    | '(' -> nextOpenParenToken ()
    | ')' -> nextCloseParenToken ()
    | '-' -> nextNumberOrSymbolToken ()
    | '0' .. '9' -> nextNumberToken ""
    | _ -> nextSymbolToken ""

(* Finally initialize CH, and return NEXT TOKEN as promised. *)

  in nextChar () ;
     nextToken ;;

(* NEXT TOKENS. Test the token scanner by reading tokens from the file whose
   pathname is PATH, and writing one-line descriptions of each token. *)

let nextTokens path =
  let nextToken = makeScanner path
  in let rec nextTokensing token =
       match token
       with CloseParenToken ->
              Printf.printf "CloseParenToken\n" ;
              nextTokensing (nextToken ()) |

            EndToken ->
              Printf.printf "EndToken\n" |

            NumberToken number ->
              Printf.printf "NumberToken %i\n" number ;
              nextTokensing (nextToken ()) |
 
            OpenParenToken ->
              Printf.printf "OpenParenToken\n" ;
              nextTokensing (nextToken ()) |

            SymbolToken string ->
              Printf.printf "SymbolToken \"%s\"\n" string ;
              nextTokensing (nextToken ())

     in nextTokensing (nextToken ()) ;;

(* Try reading tokens from EXCLAIM. *)

nextTokens "exclaim.in" ;;

(* You should see the following printed:
   
   OpenParenToken
   SymbolToken "define"
   SymbolToken "!"
   OpenParenToken
   SymbolToken "lambda"
   OpenParenToken
   SymbolToken "n"
   CloseParenToken
   OpenParenToken
   SymbolToken "if"
   OpenParenToken
   SymbolToken "="
   SymbolToken "n"
   NumberToken 0
   CloseParenToken
   NumberToken 1
   OpenParenToken
   SymbolToken "∗"
   SymbolToken "n"
   OpenParenToken
   SymbolToken "!"
   OpenParenToken
   SymbolToken "−"
   SymbolToken "n"
   NumberToken 1
   CloseParenToken
   CloseParenToken
   CloseParenToken
   CloseParenToken
   CloseParenToken
   CloseParenToken
   EndToken

*)
