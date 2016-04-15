#pragma once


#include "Node.hpp"

namespace exc {


template <class ArithmeticT, class Operator, const char* name, const char* op1desc, const char* op2desc, const char* resdesc>
class BinaryArithmeticNode
	: public InputPortConfig<ArithmeticT, ArithmeticT>,
	public OutputPortConfig < ArithmeticT >
{
public:
	BinaryArithmeticNode() {
		GetInput<0>().AddObserver(this);
		GetInput<1>().AddObserver(this);
	}

	void Update() override {
		ArithmeticT a = GetInput<0>().Get();
		ArithmeticT b = GetInput<1>().Get();
		GetOutput<0>().Set(Operator()(a, b));
	}

	void Notify(InputPortBase* sender) override {
		Update();
	}

	static std::string Info_GetName() {
		return name;
	}

	static const std::vector<std::string>& Info_GetInputNames() {
		static std::vector<std::string> names = {
			op1desc,
			op2desc
		};
		return names;
	}
	static const std::vector<std::string>& Info_GetOutputNames() {
		static std::vector<std::string> names = {
			resdesc
		};
		return names;
	}
};

// Operators
struct AddOperator {
	template <class T>
	auto operator()(T t, T u) {
		return t + u;
	}
};

struct SubtractOperator {
	template <class T>
	auto operator()(T t, T u) {
		return t - u;
	}
};

struct MultiplyOperator {
	template <class T>
	auto operator()(T t, T u) {
		return t * u;
	}
};

struct DivideOperator {
	template <class T>
	auto operator()(T t, T u) {
		return t / u;
	}
};

struct ModuloOperator {
private:
	class FloatDummy { friend struct ModuloOperator; FloatDummy() = default; };
	class IntDummy { friend struct ModuloOperator; IntDummy() = default; };
public:
	template <class T, class = std::enable_if<std::is_integral<T>::value>::type>
	auto operator()(T t, T u, IntDummy = IntDummy()) {
		return t % u;
	}
	template <class T, class = std::enable_if<std::is_floating_point<T>::value>::type>
	auto operator()(T t, T u, FloatDummy = FloatDummy()) {
		return std::remainder(t, u);
	}
};

struct PowerOperator {
	template <class T>
	T operator()(T t, T u) {
		return std::pow(t, u);
	}
};

struct LogarithmOperator {
	template <class T>
	T operator()(T t, T u) {
		return std::log(t) / std::log(u);
	}
};



// Strings
struct AddStrings {
	static constexpr const char Name[] = "Add:Adds the two inputs";
	static constexpr const char R[] = "R:A+B";
	static constexpr const char A[] = "A";
	static constexpr const char B[] = "B";
};

struct SubtractStrings {
	static constexpr const char Name[] = "Subtract:Subtracts the second input from the first";
	static constexpr const char A[] = "A";
	static constexpr const char B[] = "B";
	static constexpr const char R[] = "R:A-B";
};

struct MultiplyStrings {
	static constexpr const char Name[] = "Multiply:Multiplies the two inputs";
	static constexpr const char A[] = "A";
	static constexpr const char B[] = "B";
	static constexpr const char R[] = "R:A*B";
};

struct DivideStrings {
	static constexpr const char Name[] = "Divide:Divides the first input by the second";
	static constexpr const char A[] = "A";
	static constexpr const char B[] = "B";
	static constexpr const char R[] = "R:A/B";
};

struct ModuloStrings {
	static constexpr const char Name[] = "Modulo:Divides the first input by the second, and takes the remainder";
	static constexpr const char A[] = "A";
	static constexpr const char B[] = "B";
	static constexpr const char R[] = "R:A%B";
};

struct PowerStrings {
	static constexpr const char Name[] = "Power:Raises the first input to the power of the second input";
	static constexpr const char A[] = "Base";
	static constexpr const char B[] = "Power";
	static constexpr const char R[] = "R:Base^Power";
};

struct LogarithmStrings {
	static constexpr const char Name[] = "LogB:Takes the base-<Base> logarithm of Argument";
	static constexpr const char A[] = "Argument";
	static constexpr const char B[] = "Base";
	static constexpr const char R[] = "R:LogBase(Argument)";
};

// Node templates
template <class T>
using AddNode = BinaryArithmeticNode<
	T,
	AddOperator,
	AddStrings::Name,
	AddStrings::A,
	AddStrings::B,
	AddStrings::R>;

template <class T>
using SubtractNode = BinaryArithmeticNode<
	T,
	SubtractOperator,
	SubtractStrings::Name,
	SubtractStrings::A,
	SubtractStrings::B,
	SubtractStrings::R>;

template <class T>
using MultiplyNode = BinaryArithmeticNode<
	T,
	MultiplyOperator,
	MultiplyStrings::Name,
	MultiplyStrings::A,
	MultiplyStrings::B,
	MultiplyStrings::R>;

template <class T>
using DivideNode = BinaryArithmeticNode<
	T,
	DivideOperator,
	DivideStrings::Name,
	DivideStrings::A,
	DivideStrings::B,
	DivideStrings::R>;

template <class T>
using ModuloNode = BinaryArithmeticNode<
	T,
	ModuloOperator,
	ModuloStrings::Name,
	ModuloStrings::A,
	ModuloStrings::B,
	ModuloStrings::R>;

template <class T>
using PowerNode = BinaryArithmeticNode<
	T,
	PowerOperator,
	PowerStrings::Name,
	PowerStrings::A,
	PowerStrings::B,
	PowerStrings::R>;

template <class T>
using LogarithmNode = BinaryArithmeticNode<
	T,
	LogarithmOperator,
	LogarithmStrings::Name,
	LogarithmStrings::A,
	LogarithmStrings::B,
	LogarithmStrings::R>;

// Actual nodes
using FloatAdd = AddNode<float>;
using FloatSubtract = SubtractNode<float>;
using FloatMultiply = MultiplyNode<float>;
using FloatDivide = DivideNode<float>;
using FloatModulo = ModuloNode<float>;
using FloatPower = PowerNode<float>;
using FloatLogarithm = LogarithmNode<float>;

using IntAdd = AddNode<int>;
using IntSubtract = SubtractNode<int>;
using IntMultiply = MultiplyNode<int>;
using IntDivide = DivideNode<int>;
using IntModulo = ModuloNode<int>;

#ifdef ENABLE_HIGH_PRECISION_NODES
using FloatAdd = AddNode<double>;
using FloatSub = SubtractNode<double>;
using FloatMultiply = MultiplyNode<double>;
using FloatDivide = DivideNode<double>;
using FloatModulo = ModuloNode<double>;

using FloatAdd = AddNode<long long>;
using FloatSub = SubtractNode<long long>;
using FloatMultiply = MultiplyNode<long long>;
using FloatDivide = DivideNode<long long>;
using FloatModulo = ModuloNode<long long>;
#endif



} // namespace exc