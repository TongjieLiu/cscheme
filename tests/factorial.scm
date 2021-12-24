; factorial.scm -- a simple test for cscheme interpreter

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

(define (fac n)
	(if (= n 0)
		1
		(* (fac (- n 1)) n)))




(printn "fac(0) =" (fac 0))
(printn "fac(1) =" (fac 1))
(printn "fac(2) =" (fac 2))
(printn "fac(3) =" (fac 3))
(printn "fac(4) =" (fac 4))
(printn "fac(5) =" (fac 5))
(printn "fac(6) =" (fac 6))
(printn "fac(7) =" (fac 7))
(printn "fac(8) =" (fac 8))
(printn "fac(9) =" (fac 9))
(printn "fac(10) =" (fac 10))
