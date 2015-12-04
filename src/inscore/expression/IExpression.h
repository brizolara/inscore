﻿#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

#include <string>
#include <iostream>
#include <map>

#include "smartpointer.h"

#include "Operators.h"
#include "evaluator.h"


namespace inscore{

class IExprOperator;
typedef libmapping::SMARTP<IExprOperator> SIExprOperator;

class OperatorPrototype;

class IExprArgBase;
typedef libmapping::SMARTP<IExprArgBase> SIExprArg;

class IExpression;
typedef libmapping::SMARTP<IExpression> SIExpression;

class IExpression: public libmapping::smartable{
private:
	SIExprArg fRootNode;
	std::string fDefinition;

public:
	static SIExpression create(const std::string &definition, const SIExprArg &fRootNode);
	static SIExpression createEmpty();

	std::string definition() const {return fDefinition;}
	const SIExprArg& rootNode() const {return fRootNode;}
	void setRootNode(SIExprArg rootNode){fRootNode = SIExprArg(rootNode);}

protected:
	IExpression(const std::string &definition, const SIExprArg &rootNode);
};

//____________________________________________________________
/*!
 * \brief Evaluable expression made of an operator prototype and two arguments.
 */
class IExprOperator: public libmapping::smartable{
private:
	const OperatorPrototype* fOperatorPrototype;
	SIExprArg fArg1, fArg2;

public:
	IExprOperator(const OperatorPrototype* operatorPrototype, SIExprArg arg1, SIExprArg arg2);

	SIExprArg& arg1() {return fArg1;}
	SIExprArg& arg2() {return fArg2;}

	const SIExprArg constArg1() const {return fArg1;}
	const SIExprArg constArg2() const {return fArg2;}

	bool dynamicEval() const;

	const OperatorPrototype* operatorPrototype() const {return fOperatorPrototype;}

    std::string getName() const;
};


//____________________________________________________________________________________________
/*!
 * \brief base classes for all IExpression arguments , it hide the template of IExprArg<T>
 */
class IExprArgBase: public libmapping::smartable, public evaluable{
protected:
	bool fCopyEval, fDynamicEval;
	std::string* fEvaluated;

public:
	virtual std::string accept(evaluator* e) =0;
	virtual std::string accept(constEvaluator* e) const = 0;

	void switchToDynamic(bool dynamicEval = true){fDynamicEval = dynamicEval;}
	void switchToCopy(bool copyEval = true){fCopyEval = copyEval;}
	inline bool copyEval() const {return fCopyEval;}
	inline bool dynamicEval() const {return fDynamicEval;}
	inline bool pureStaticEval() const {return !fCopyEval && !fDynamicEval;}

	virtual SIExprArg copy() const =0;

	inline std::string getEvaluated() const {return *fEvaluated;}
	inline void setEvaluated(std::string evaluated){*fEvaluated = evaluated;}
	inline std::string* evaluated() const {return fEvaluated;}	//as evaluated is more a buffer than a real attribute
																//the pointer isn't const so evaluated can be change even in a const IExpression
	virtual void recursiveClearEvaluated()=0;

	virtual ~IExprArgBase(){delete fEvaluated;}

protected:
	IExprArgBase();
};




//____________________________________________________________________________________________
template <typename argType>
/*!
 * \brief Containers class used to store any arguments of any type passed to an operator.
 */
class IExprArg: public IExprArgBase{
private:
    argType fArg;

public:
	IExprArg(argType arg): IExprArgBase(), fArg(arg){}
	argType getArg(){return fArg;}

	SIExprArg copy() const {
		IExprArgBase* r = new IExprArg<argType>(fArg);
		if(fDynamicEval)
			r->switchToDynamic();
		if(fCopyEval)
			r->switchToCopy();
		r->setEvaluated(getEvaluated());
		return r;
	}

	void recursiveClearEvaluated(){fEvaluated->clear();}

    /*!
     * \brief accept the visit from an evaluator
     * \param e the evaluator
     * \return the evaluated string
     */
	std::string accept(evaluator* e){
		return e->eval(fArg, this);
    }
	std::string accept(constEvaluator *e) const {
		return e->eval(fArg, this);
	}

};

std::ostream&	operator << (std::ostream& out, const SIExpression& exprArg);
std::ostream&	operator << (std::ostream& out, const SIExprArg& exprArg);
std::ostream&	operator << (std::ostream& out, const SIExprOperator& exprArg);

} //end namespace






#endif // EXPRESSIONNODE_H
