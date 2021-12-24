; fibonacci.scm -- a simple test for cscheme interpreter

; Copyright (C) 2021 Tongjie Liu <tongjieandliu@gmail.com>.

; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.

; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

(define (fib n)
	(do_fib n 0 0 1))


(define (do_fib n index a b)
	(if (= index n)
		a
		(do_fib n (+ index 1) b (+ a b))))




(printn "fib(0) =" (fib 0))
(printn "fib(1) =" (fib 1))
(printn "fib(2) =" (fib 2))
(printn "fib(3) =" (fib 3))
(printn "fib(4) =" (fib 4))
(printn "fib(5) =" (fib 5))
(printn "fib(6) =" (fib 6))
(printn "fib(7) =" (fib 7))
(printn "fib(8) =" (fib 8))
(printn "fib(9) =" (fib 9))
(printn "fib(10) =" (fib 10))
