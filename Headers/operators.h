/*
  This is where you define a new operator so that the compiler and
  the run-time system can see it
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.

 */

infixOp(". ",1899,1900,1900,"statement separator")
infixOp("@",1254,1255,1255,"meta annotation")
infixOp(":",1249,1250,1249,"type annotation")
infixOp("<=",1249,1250,1249,"class rule arrow")
infixOp("~~",1239,1240,1240,"quantifier")
infixOp("|:",1234,1235,1234,"constrained type")
infixOp("::=",1230,1231,1230,"user type definition")
infixOp(":-",1219,1220,1219,"clause arrow")
infixOp("|",1219,1220,1220,"type union and disjunction")
infixOp("?",1199,1200,1199,"conditional operator")
infixOp("-->",1199,1200,1199,"grammar rule")
infixOp("->>",1199,1200,1199,"dependent type marker")
infixOp("*>",1151,1152,1151,"all solutions")
infixOp("||",1059,1060,1059,"bag of constructor")
infixOp("::-",1004,1005,1004,"semantic guard")
infixOp(",",999,1000,1000,"tupling, conjunction")
infixOp(",..",999,1000,1000,"list cons")
infixOp("<=>",949,950,949,"class constructor type")
infixOp("=>",949,950,949,"function arrow")
infixOp("<~",949,949,948,"type rule")
infixOp("~>",949,949,948,"type alias definition")
infixOp(">>=",899,900,900,"monadic bind")
infixOp("->",899,900,899,"map entry")
infixOp("=",899,900,899,"unifies predicate")
infixOp("==",899,900,899,"equality predicate")
infixOp("\\=",899,900,899,"not unifyable")
infixOp("\\==",899,900,899,"not equals")
infixOp("!=",899,900,899,"not equal")
infixOp("<",899,900,899,"less than")
infixOp("=<",899,900,899,"less than or equal")
infixOp(">",899,900,899,"greater than")
infixOp(">=",899,900,899,"greater than or equal")
infixOp("in",899,900,899,"list membership")
infixOp("<>",799,800,800,"list append")
infixOp("//",800,800,799,"map over")
infixOp("///",800,800,799,"indexed map over")
infixOp("^/",800,800,799,"filter")
infixOp("^//",800,800,799,"filter map")
infixOp("#",759,760,759,"package separator")
infixOp("+",720,720,719,"addition")
infixOp("-",720,720,719,"subtraction")
infixOp(".|.",720,720,719,"bitwise or")
infixOp(".^.",720,720,719,"bitwise xor")
infixOp("*",700,700,699,"multiplication")
infixOp("/",700,700,699,"division")
infixOp(".&.",700,700,699,"bitwise and")
infixOp("%",700,700,699,"modulo")
infixOp("**",600,600,599,"exponentiation")
infixOp(".<<.",600,600,599,"shift left")
infixOp(".>>.",600,600,599,"logical shift right")
infixOp(".>>>.",600,600,599,"arithmetic shift right")
infixOp(".#.",600,600,599,"test nth bit")
infixOp("%%",499,500,499,"grammar parse")
infixOp("~",489,499,489,"grammar remainder")
infixOp(".",450,450,449,"object access")
lastInfOp
infixOp("::",399,400,399,"type coercion")

prefixOp("private",1700,1699,"private program")
prefixOp("public",1700,1699,"public program")
prefixOp("assert", 1260,1259,"assert condition")
prefixOp("show", 1260,1259,"display debug message")
prefixOp("contract",1260,1259,"contract definition")
prefixOp("implementation",1260,1259,"contract implementation")
prefixOp("@",1255,1255,"meta annotation")
prefixOp("type",1260,1259,"type definition")
prefixOp("all",1239,1238,"universal quantifier")
prefixOp("exists",1239,1238,"existential quantifier")
prefixOp("\\+",905,904,"logical negation")
prefixOp("open",900,899,"import object")
prefixOp("import",900,899,"import module")
prefixOp("return",899,890,"wrap value in monad")
prefixOp("raise",899,890,"error return in monad")
prefixOp(".~.",650,649,"bitwise 1's complement")
prefixOp("-",300,299,"arithmetic negation")
lastPreOp
prefixOp(".",1,0,"label prefix")

postfixOp(". ",1899,1900,"statement terminator")
postfixOp(";",1149,1150,"action terminator")
postfixOp("!",904,905,"one solution operator")
postfixOp("+",759,760,"lookahead in grammar rule")
postfixOp("^",49,50,"output mode marker")
postfixOp("?",49,50,"input mode marker")
postfixOp("^?",49,50,"bidirectional mode marker")
lastPostOp
postfixOp("?^",49,50,"bidirectional mode marker")

token("$","Used for curried functions and types")
