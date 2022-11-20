#ifndef ACCEL_MATH_H
#define ACCEL_MATH_H

#include <cstddef>
#include <cmath>

#include <ostream>
#include <array>
#include <numeric>

namespace accel
{
	namespace details
	{
		struct radians_trait {};
		struct degrees_trait {};

		template <typename T, typename = void> struct has_value : std::false_type {};
		template <typename T> struct has_value<T, decltype((void)T::value, void())> : std::true_type {};
	}

	namespace constants
	{
		template<typename T> constexpr static T pi() { return static_cast<T>(3.14159265358979323846264338327950288L); }
		template<typename T> constexpr static T half_pi() { return pi<T>() / T(2); }
		template<typename T> constexpr static T two_pi() { return pi<T>() * T(2); }
		template<typename T> constexpr static T rad_to_deg() { return T(180) / pi<T>(); }
		template<typename T> constexpr static T deg_to_rad() { return pi<T>() / T(180); }
	}

	template<typename T, typename UnitTrait = details::radians_trait>
	class angle
	{
	public:
		constexpr static angle pi() { return angle<T, details::radians_trait>(constants::pi<T>()); }
		constexpr static angle asin(T value) { return angle<T, details::radians_trait>(std::asin(value)); }
		constexpr static angle acos(T value) { return angle<T, details::radians_trait>(std::acos(value)); }
		constexpr static angle atan(T value) { return angle<T, details::radians_trait>(std::atan(value)); }
		constexpr static angle atanh(T value) { return angle<T, details::radians_trait>(std::atanh(value)); }
		constexpr static angle atan2(T x, T y) { return angle<T, details::radians_trait>(std::atan2(x, y)); }

		constexpr angle() : m_angle() {}
		constexpr explicit angle(T value) : m_angle(value) {}
		
		template<typename OtherTraitT>
		constexpr angle(const angle<T, OtherTraitT>& other)
		{
			if constexpr (std::is_same_v<OtherTraitT, UnitTrait>)
			{
				m_angle = other.m_angle;
			}
			else
			{
				if constexpr (std::is_same_v<UnitTrait, details::radians_trait>)
					m_angle = static_cast<T>(other) * constants::deg_to_rad<T>();
				else
					m_angle = static_cast<T>(other) * constants::rad_to_deg<T>();
			}
		}

		constexpr angle(const angle& value) = default;
		constexpr angle& operator=(const angle& value) = default;
		constexpr angle(angle&&) = default;
		constexpr angle& operator=(angle&&) = default;
		
		template<typename OtherTrait> constexpr angle& operator=(const angle<T, OtherTrait>& other) { *this = angle<T, OtherTrait>(other); }

		constexpr void normalize()
		{
			if constexpr (std::is_same_v<UnitTrait, details::radians_trait>)
				m_angle = std::fmod(m_angle, constants::two_pi<T>());
			else
				m_angle = std::fmod(m_angle, T(360));
		}

		constexpr T sin() const
		{
			if constexpr (std::is_same_v<UnitTrait, details::degrees_trait>)
				return std::sin(angle<T>(*this));
			else
				return std::sin(m_angle);
		}
		constexpr T cos() const
		{
			if constexpr (std::is_same_v<UnitTrait, details::degrees_trait>)
				return std::cos(angle<T>(*this));
			else
				return std::cos(m_angle);
		}
		constexpr T tan() const 
		{
			if constexpr (std::is_same_v<UnitTrait, details::degrees_trait>)
				return std::tan(angle<T>(*this));
			else
				return std::tan(m_angle); 
		}

		template<typename OtherTrait> constexpr bool operator==(const angle<T, OtherTrait>& other) { return m_angle == angle<T, UnitTrait>(other); }
		template<typename OtherTrait> constexpr bool operator!=(const angle<T, OtherTrait>& other) { return !operator==(other); }
		constexpr bool operator==(const T& value) const { return m_angle == value; }
		constexpr bool operator!=(const T& value) const { return !operator==(value); }
		
		constexpr bool operator>(const angle& other) const { return m_angle > other.m_angle; }
		constexpr bool operator<(const angle& other) const { return m_angle < other.m_angle; }
		constexpr bool operator>=(const angle& other) const { return m_angle >= other.m_angle; }
		constexpr bool operator<=(const angle& other) const { return m_angle <= other.m_angle; }

		constexpr angle operator+(const angle& other) const { return { m_angle + other.m_angle }; }
		constexpr angle operator-(const angle& other) const { return { m_angle - other.m_angle }; }
		constexpr angle operator*(const angle& other) const { return { m_angle * other.m_angle }; }
		constexpr angle operator/(const angle& other) const { return { m_angle / other.m_angle }; }
		constexpr angle operator%(const angle& other) const { return { m_angle % other.m_angle }; }
		constexpr angle& operator+=(const angle& other)
		{
			m_angle += other.m_angle;
			return *this;
		}
		constexpr angle& operator-=(const angle& other)
		{
			m_angle -= other.m_angle;
			return *this;
		}
		constexpr angle& operator*=(const angle& other)
		{
			m_angle *= other.m_angle;
			return *this;
		}
		constexpr angle& operator/=(const angle& other)
		{
			m_angle /= other.m_angle;
			return *this;
		}
		constexpr angle& operator%=(const angle& other)
		{
			m_angle %= other.m_angle;
			return *this;
		}
		template<typename ScalarT> constexpr angle operator*(const ScalarT& value) const { return angle(m_angle * value); }
		template<typename ScalarT> constexpr angle operator/(const ScalarT& value) const { return angle(m_angle / value); }

		constexpr angle operator-() const { return angle(-m_angle); }
		constexpr operator T() const { return m_angle; }

	private:
		T m_angle;
	};
	using radiansf = angle<float, details::radians_trait>;
	using radiansd = angle<double, details::radians_trait>;
	using degreesf = angle<float, details::degrees_trait>;
	using degreesd = angle<double, details::degrees_trait>;

	template<std::size_t Dimensions, typename T> class point;

	template<std::size_t Dimensions, typename T>
	class size
	{
	public:
		friend class point<Dimensions, T>;

		using storage_type = std::array<T, Dimensions>;
		using value_type = T;
		using iterator = typename storage_type::iterator;
		using const_iterator = typename storage_type::const_iterator;

		template<typename... Ts>
		constexpr size(Ts... values) : m_data{ values... } {}

		// Copyable
		constexpr size(const size&) = default;
		constexpr size& operator=(const size&) = default;

		// Movable
		constexpr size(size&&) = default;
		constexpr size& operator=(size&&) = default;

		// Iterators
		constexpr iterator begin() { return m_data.begin(); }
		constexpr iterator end() { return m_data.end(); }
		constexpr const_iterator cbegin() const { return m_data.cbegin(); }
		constexpr const_iterator cend() const { return m_data.cend(); }

		// Accessors
		constexpr const T& width() const { return m_data[0]; }
		constexpr T& width() { return m_data[0]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr const T& height() const { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr T& height() { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr const T& depth() const { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr T& depth() { return m_data[2]; }

		// Operators
		constexpr bool operator==(const size& other) const { return m_data == other.m_data; }
		constexpr bool operator!=(const size& other) const { return !operator==(other); }

		constexpr size operator+(const size& other) { return sum(other, std::make_index_sequence<Dimensions>{}); }
		constexpr size operator-(const size& other) { return difference(other, std::make_index_sequence<Dimensions>{}); }
		constexpr size operator*(const T& value) { return product(value, std::make_index_sequence<Dimensions>{}); }
		constexpr size operator/(const T& value) { return quotient(value, std::make_index_sequence<Dimensions>{}); }
		constexpr size& operator+=(const size& other) 
		{ 
			*this = sum(other, std::make_index_sequence<Dimensions>{}); 
			return *this;
		}
		constexpr size& operator-=(const size& other)
		{
			*this = difference(other, std::make_index_sequence<Dimensions>{});
			return *this;
		}

		// Data access
		constexpr T operator[](std::size_t index) const { return m_data[index]; }
		constexpr T& operator[](std::size_t index) { return m_data[index]; }
		constexpr const T& at(std::size_t index) const { return m_data.at(index); }
		constexpr T& at(std::size_t index) { return m_data.at(index); }
		constexpr const T* data() const { return m_data.data(); }
		constexpr T* data() { return m_data.data(); }

	private:
		storage_type m_data;

		template<std::size_t... Indices> constexpr size sum(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] + other[Indices])... }; }
		template<std::size_t... Indices> constexpr size difference(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] - other[Indices])... }; }

		template<std::size_t... Indices> constexpr size product(const T& value, std::index_sequence<Indices...>) const { return { (m_data[Indices] * value)... }; }
		template<std::size_t... Indices> constexpr size quotient(const T& value, std::index_sequence<Indices...>) const { return { (m_data[Indices] / value)... }; }
	};
	using size2f = size<2, float>;
	using size2d = size<2, double>;
	using size2i = size<2, int>;
	using size2u = size<2, unsigned int>;
	using size3f = size<3, float>;
	using size3d = size<3, double>;
	using size3i = size<3, int>;
	using size3u = size<3, unsigned int>;

	template<std::size_t Dimensions, typename T> class vector;

	template<std::size_t Dimensions, typename T = float>
	class point
	{
	public:
		using storage_type = std::array<T, Dimensions>;
		using value_type = T;
		using iterator = typename storage_type::iterator;
		using const_iterator = typename storage_type::const_iterator;

		template<typename... Ts>
		constexpr point(Ts... values) : m_data{ values... } {}

		// Copyable
		constexpr point(const point&) = default;
		constexpr point& operator=(const point&) = default;

		// Movable
		constexpr point(point&&) = default;
		constexpr point& operator=(point&&) = default;

		// Iterators
		constexpr iterator begin() { return m_data.begin(); }
		constexpr iterator end() { return m_data.end(); }
		constexpr const_iterator cbegin() const { return m_data.cbegin(); }
		constexpr const_iterator cend() const { return m_data.cend(); }

		// Accessors
		constexpr const T& x() const { return m_data[0]; }
		constexpr T& x() { return m_data[0]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr const T& y() const { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr T& y() { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr const T& z() const { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr T& z() { return m_data[2]; }

		// Methods
		constexpr vector<Dimensions, T> vector_to(const point& other) const { return (*this - other).operator accel::vector<Dimensions, T>(); }

		// Operators
		constexpr bool operator==(const point& other) const { return m_data == other.m_data; }
		constexpr bool operator!=(const point& other) const { return !operator==(other); }

		constexpr point operator+(const point& other) const { return sum(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr point operator+(const size<Dimensions, T>& other) const { return sum(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr point operator-(const point& other) const { return difference(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr point operator-(const size<Dimensions, T>& other) const { return difference(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr point operator*(const T& value) const { return product(value, std::make_index_sequence<Dimensions>{}); }
		constexpr point operator/(const T& value) const { return quotient(value, std::make_index_sequence<Dimensions>{}); }
		constexpr point& operator+=(const point& other)
		{
			*this = sum(other.m_data, std::make_index_sequence<Dimensions>{});
			return *this;
		}
		constexpr point& operator-=(const point& other)
		{
			*this = difference(other.m_data, std::make_index_sequence<Dimensions>{});
			return *this;
		}

		constexpr operator vector<Dimensions, T>() const { return vector<Dimensions, T>(m_data);  }

		// Data access
		constexpr T operator[](std::size_t index) const { return m_data[index]; }
		constexpr T& operator[](std::size_t index) { return m_data[index]; }
		constexpr const T& at(std::size_t index) const { return m_data.at(index); }
		constexpr T& at(std::size_t index) { return m_data.at(index); }
		constexpr const T* data() const { return m_data.data(); }
		constexpr T* data() { return m_data.data(); }

	private:
		storage_type m_data;

		template<std::size_t... Indices> constexpr point sum(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] + other[Indices])... }; }
		template<std::size_t... Indices> constexpr point difference(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] - other[Indices])... }; }

		template<std::size_t... Indices> constexpr point product(const T& value, std::index_sequence<Indices...>) const { return { (m_data[Indices] * value)... }; }
		template<std::size_t... Indices> constexpr point quotient(const T& value, std::index_sequence<Indices...>) const { return { (m_data[Indices] / value)... }; }
	};
	using point2f = point<2, float>;
	using point2d = point<2, double>;
	using point2i = point<2, int>;
	using point2u = point<2, unsigned int>;
	using point3f = point<3, float>;
	using point3d = point<3, double>;
	using point3i = point<3, int>;
	using point3u = point<3, unsigned int>;
	using point4f = point<4, float>;
	using point4d = point<4, double>;
	using point4i = point<4, int>;
	using point4u = point<4, unsigned int>;

	template<typename T>
	class rectangle
	{
	public:
		using storage_type = std::array<T, 4>;
		using value_type = T;
		using iterator = typename storage_type::iterator;
		using const_iterator = typename storage_type::const_iterator;

		rectangle() = default;
		rectangle(const point<2, T>& top_left, const size<2, T>& size) : m_values{ top_left.y(), top_left.x(), top_left.y() + size.height(), top_left.x() + size.width() } {}
		rectangle(T top, T left, T bottom, T right) : m_values { top, left, bottom, right } {}

		// Copyable
		constexpr rectangle(const rectangle&) = default;
		constexpr rectangle& operator=(const rectangle&) = default;

		// Movable
		constexpr rectangle(rectangle&&) = default;
		constexpr rectangle& operator=(rectangle&&) = default;

		// Iterators

		constexpr bool operator==(const rectangle& other) const { return m_values == other.m_values; }
		constexpr bool operator!=(const rectangle& other) const { return !operator==(other); }

		constexpr T top() const { return m_values[0]; }
		constexpr T& top() { return m_values[0]; }
		constexpr T left() const { return m_values[1]; }
		constexpr T& left() { return m_values[1]; }
		constexpr T bottom() const { return m_values[2]; }
		constexpr T& bottom() { return m_values[2]; }
		constexpr T right() const { return m_values[3]; }
		constexpr T& right() { return m_values[3]; }

		constexpr T height() const { return m_values[2] - m_values[0]; }
		constexpr T width() const { return m_values[3] - m_values[1]; }
		constexpr size<2, T> size() const { return accel::size<2, T>(width(), height()); }

		constexpr point<2, T> top_left() const { return accel::point<2, T>(m_values[1], m_values[0]); }
		constexpr point<2, T> top_right() const { return accel::point<2, T>(m_values[3], m_values[0]); }
		constexpr point<2, T> bottom_left() const { return accel::point<2, T>(m_values[1], m_values[2]); }
		constexpr point<2, T> bottom_right() const { return accel::point<2, T>(m_values[3], m_values[2]); }

		constexpr bool valid() const { return width() > T(0) && height() > T(0); }
		constexpr void offset(const accel::size<2, T>& size) 
		{ 
			this->top() += size.height();
			this->bottom() += size.height();
			this->left() += size.width();
			this->right() += size.width();
		}
		constexpr void inset(const accel::size<2, T>& size) { return inset(size.height(), size.width(), size.height(), size.width()); }
		constexpr void inset(T top, T left, T bottom, T right)
		{
			this->top() += + top;
			this->bottom() -= bottom;
			this->left() += left;
			this->right() -= right;
		}
		constexpr void pad(const accel::size<2, T>& size) { return inset(accel::size<2, T>(-size.width(), -size.height())); }
		constexpr void pad(T top, T left, T bottom, T right) { return inset(-top, -left, -bottom, -right); }
		constexpr bool intersects(const rectangle& other) { return intersection(other).valid();}
		constexpr rectangle intersection(const rectangle& other)
		{
			return rectangle
			(
				std::max(top(), other.top()),
				std::max(left(), other.left()),
				std::min(bottom(), other.bottom()),
				std::min(right(), other.right())
			);
		}

	private:
		storage_type m_values;
	};
	using rectanglef = rectangle<float>;
	using rectangled = rectangle<double>;
	using rectanglei = rectangle<int>;
	using rectangleu = rectangle<unsigned int>;

	template <std::size_t Rows, std::size_t Columns, typename T = float>
	class matrix;
	using matrix2f = matrix<2, 2, float>;
	using matrix2d = matrix<2, 2, double>;
	using matrix3f = matrix<3, 3, float>;
	using matrix3d = matrix<3, 3, double>;
	using matrix4f = matrix<4, 4, float>;
	using matrix4d = matrix<4, 4, double>;
	template<std::size_t Rows, std::size_t Columns, typename T>
	std::ostream& operator<<(std::ostream&, const matrix<Rows, Columns, T>&);
	template<std::size_t Rows, std::size_t Columns, typename T>
	class matrix
	{
	public:
		using storage_type = std::array<std::array<T, Columns>, Rows>;
		using value_type = T;
		using iterator = typename storage_type::iterator;
		using const_iterator = typename storage_type::const_iterator;

		template<typename = typename std::enable_if_t<Rows == Columns>> constexpr static matrix identity();
		template<typename = typename std::enable_if_t<Rows == 3 && Columns == 3>> constexpr static matrix translate(const vector<2,T>& position);
		template<typename = typename std::enable_if_t<Rows == 3 && Columns == 3>> constexpr static matrix scale(const size<2, T>& value);
		template<typename = typename std::enable_if_t<Rows == 3 && Columns == 3>> constexpr static matrix rotate(const angle<T>& value);
		template<typename = typename std::enable_if_t<Rows == 3 && Columns == 3>> constexpr static matrix shear(const vector<2, T>& value);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix translate(const vector<3, T>& position);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix scale(const vector<3, T>& position);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix rotate_x(const angle<T>& value);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix rotate_y(const angle<T>& value);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix rotate_z(const angle<T>& value);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix perspective(const angle<T>& horizontal_fov, T aspect_ratio, T near_z, T far_z);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix perspective_v(const angle<T>& vertical_fov, T aspect_ratio, T near_z, T far_z);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix lookat(const point<3, T>& target, const point<3, T>& at, const vector<3, T>& up);
		template<typename = typename std::enable_if_t<Rows == 4 && Columns == 4>> constexpr static matrix orthographic(const rectangle<T>& rect, T z_near, T z_far);

		constexpr static std::size_t rows();
		constexpr static std::size_t columns();
		constexpr static std::size_t size();

		constexpr matrix(const std::array<T, Rows * Columns>& data);
		constexpr matrix(std::array<T, Rows * Columns>&& data);
		template<typename... Ts> constexpr matrix(Ts ...values);

		// Copyable
		constexpr matrix(const matrix&) = default;
		constexpr matrix& operator=(const matrix&) = default;

		// Movable
		constexpr matrix(matrix&&) = default;
		constexpr matrix& operator=(matrix&&) = default;

		constexpr iterator begin();
		constexpr iterator end();
		constexpr const_iterator cbegin() const;
		constexpr const_iterator cend() const;

		// Methods
		constexpr vector<Columns, T> row(std::size_t index) const;
		constexpr vector<Rows, T> column(std::size_t index) const;
		constexpr matrix transposed() const;

		constexpr bool operator==(const matrix& other) const;
		constexpr bool operator!=(const matrix& other) const;

		template<std::size_t N> constexpr matrix<Rows, N, T> operator*(const matrix<Columns, N, T>& other) const;

		constexpr const T& at(std::size_t row, std::size_t column) const;
		constexpr T& at(std::size_t row, std::size_t column);
		constexpr const T& at(std::size_t index) const;
		constexpr T& at(std::size_t index);
		constexpr const T* data() const;
		constexpr T* data();

		constexpr T operator()(std::size_t row, std::size_t column) const;
		constexpr T& operator()(std::size_t row, std::size_t column);
		constexpr T operator()(std::size_t index) const;
		constexpr T& operator()(std::size_t index);

		template <typename T1>
		friend std::ostream& operator<< <>(std::ostream& stream, const matrix& m);

	protected:
		storage_type m_data;
	};

	struct swizzle_x { constexpr static std::size_t index = 0; };
	struct swizzle_y { constexpr static std::size_t index = 1; };
	struct swizzle_z { constexpr static std::size_t index = 2; };
	struct swizzle_w { constexpr static std::size_t index = 3; };
	struct swizzle_zero { constexpr static int value = 0; };
	struct swizzle_one { constexpr static int value = 1; };
	using swizzle_r = swizzle_x;
	using swizzle_g = swizzle_y;
	using swizzle_b = swizzle_z;
	using swizzle_a = swizzle_w;
	template<std::size_t Dimensions, typename T = float>
	class vector
	{
	public:
		using storage_type = std::array<T, Dimensions>;
		using value_type = T;
		using iterator = typename storage_type::iterator;
		using const_iterator = typename storage_type::const_iterator;

		template<typename... Ts> constexpr vector(Ts... values) : m_data{ static_cast<T>(values)... } {}
		constexpr vector(const T& value) { m_data.fill(value); }
		constexpr vector(const vector<Dimensions - 1, T>& other, T value)
		{
			std::copy(other.cbegin(), other.cend(), m_data.begin());
			m_data[Dimensions - 1] = value;
		}
		constexpr vector(const storage_type& storage) : m_data(storage) {}

		// Copyable
		constexpr vector(const vector&) = default;
		constexpr vector& operator=(const vector&) = default;

		// Movable
		constexpr vector(vector&&) = default;
		constexpr vector& operator=(vector&&) = default;

		// Properties
		constexpr static std::size_t size() { return Dimensions; }

		// Data access
		constexpr T operator[](std::size_t index) const { return m_data[index]; }
		constexpr T& operator[](std::size_t index) { return m_data[index]; }
		constexpr const T& at(std::size_t index) const { return m_data.at(index); }
		constexpr T& at(std::size_t index) { return m_data.at(index); }
		constexpr const T* data() const { return m_data.data(); }
		constexpr T* data() { return m_data.data(); }

		// Iterators
		constexpr iterator begin() { return m_data.begin(); }
		constexpr iterator end() { return m_data.end(); }
		constexpr const_iterator cbegin() const { return m_data.cbegin(); }
		constexpr const_iterator cend() const { return m_data.cend(); }

		// Methods
		constexpr T sum() const { return std::accumulate(m_data.cbegin(), m_data.cend(), T()); }
		constexpr T mean() const { return sum() / size(); }
		constexpr T length() const { return std::sqrt(length_squared()); }
		constexpr T length_squared() const { return dot(m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr vector normalized() const 
		{ 
			auto value = length();
			if (value == 0)
				return vector();
			else
				return quotient(value, std::make_index_sequence<Dimensions>{});
		}

		constexpr angle<T, details::radians_trait> angle(const vector& other) { return accel::angle<T>::acos(this->operator*(other) / std::sqrt(length_squared() * other.length_squared())); }
		template<typename... Ts> constexpr vector<sizeof...(Ts), T> swizzle() const
		{
			return { ([&]()
			{
				if constexpr (details::has_value<Ts>::value)
				{
					return static_cast<T>(Ts::value);
				}
				else
				{
					static_assert(Ts::index < Dimensions, "Can't index a dimension that doesn't exist.");
					return m_data[Ts::index];
				}
			}())... };
		}

		// X Y Z W accessors
		constexpr const T& x() const { return m_data[0]; }
		constexpr T& x() { return m_data[0]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr const T& y() const { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr T& y() { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr const T& z() const { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr T& z() { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 4>> constexpr const T& w() const { return m_data[3]; }
		template<typename = typename std::enable_if_t<Dimensions >= 4>> constexpr T& w() { return m_data[3]; }

		// R G B A accessors
		constexpr const T& r() const { return m_data[0]; }
		constexpr T& r() { return m_data[0]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr const T& g() const { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 2>> constexpr T& g() { return m_data[1]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr const T& b() const { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 3>> constexpr T& b() { return m_data[2]; }
		template<typename = typename std::enable_if_t<Dimensions >= 4>> constexpr const T& a() const { return m_data[3]; }
		template<typename = typename std::enable_if_t<Dimensions >= 4>> constexpr T& a() { return m_data[3]; }

		// Equality operators
		constexpr bool operator==(const vector& other) const { return m_data == other.m_data; }
		constexpr bool operator!=(const vector& other) const { return !operator==(other); }

		// Vector operators
		constexpr vector operator+(const vector& other) const { return sum(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr vector operator-(const vector& other) const { return difference(other.m_data, std::make_index_sequence<Dimensions>{}); }
		constexpr T operator*(const vector& other) const { return dot(other.m_data, std::make_index_sequence<Dimensions>{}); }
		template<typename = typename std::enable_if_t<Dimensions == 2>> constexpr T operator^(const vector& other) const 
		{ 
			return (x() * other.y()) - (y() * other.x()); 
		}
		template<typename = typename std::enable_if_t<Dimensions == 3>> constexpr vector operator^(const vector& other) const 
		{ 
			return vector(
				y() * other.z() - z() * other.y(),
				z() * other.x() - x() * other.z(),
				x() * other.y() - y() * other.x()
			); 
		}
		constexpr vector& operator+=(const vector& other)
		{
			*this = sum(other.m_data, std::make_index_sequence<Dimensions>{});
			return *this;
		}
		constexpr vector& operator-=(const vector& other)
		{
			*this = difference(other.m_data, std::make_index_sequence<Dimensions>{});
			return *this;
		}
		constexpr vector& operator*=(const vector& other)
		{
			*this = dot(other.m_data, std::make_index_sequence<Dimensions>{});
			return *this;
		}

		// Scalar operators
		template<typename ScalarT> constexpr vector operator+(const ScalarT& value) const { return sum(value, std::make_index_sequence<Dimensions>{}); }
		template<typename ScalarT> constexpr vector operator-(const ScalarT& value) const { return difference(value, std::make_index_sequence<Dimensions>{}); }
		template<typename ScalarT> constexpr vector operator*(const ScalarT& value) const { return product(value, std::make_index_sequence<Dimensions>{}); }
		template<typename ScalarT> constexpr vector operator/(const ScalarT& value) const { return quotient(value, std::make_index_sequence<Dimensions>{}); }

		// Matrix multiplication
		template<std::size_t Rows> 
		constexpr vector operator*(const matrix<Rows, Dimensions, T>& matrix) const
		{
			vector result;
			for (std::size_t row = 0; row < Rows; row++)
			{
				for (std::size_t column = 0; column < Dimensions; column++)
					result[row] = result[row] + (m_data[column] * matrix(row, column));
			}
			return result;
		}

	private:
		storage_type m_data;

		// Vector operations
		template<std::size_t... Indices> constexpr vector sum(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] + other[Indices])... }; }
		template<std::size_t... Indices> constexpr vector difference(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] - other[Indices])... }; }
		template<std::size_t... Indices> constexpr vector product(const storage_type& other, std::index_sequence<Indices...>) const{ return { (m_data[Indices] * other[Indices])... }; }
		template<std::size_t... Indices> constexpr vector quotient(const storage_type& other, std::index_sequence<Indices...>) const { return { (m_data[Indices] / other[Indices])... }; }
		template<std::size_t... Indices> constexpr T dot(const storage_type& other, std::index_sequence<Indices...>) const { return (... + (m_data[Indices] * other[Indices])); }

		// Scalar operations
		template<typename ScalarT, std::size_t... Indices> constexpr vector sum(const ScalarT& scalar, std::index_sequence<Indices...>) const { return { (m_data[Indices] + scalar)... }; }
		template<typename ScalarT, std::size_t... Indices> constexpr vector difference(const ScalarT& scalar, std::index_sequence<Indices...>) const { return { (m_data[Indices] - scalar)... }; }
		template<typename ScalarT, std::size_t... Indices> constexpr vector product(const ScalarT& scalar, std::index_sequence<Indices...>) const { return { (m_data[Indices] * scalar)... }; }
		template<typename ScalarT, std::size_t... Indices> constexpr vector quotient(const ScalarT& scalar, std::index_sequence<Indices...>) const { return { (m_data[Indices] / scalar)... }; }
	};
	using vector2f = vector<2, float>;
	using vector2d = vector<2, double>;
	using vector2i = vector<2, int>;
	using vector2u = vector<2, unsigned int>;
	using vector3f = vector<3, float>;
	using vector3d = vector<3, double>;
	using vector3i = vector<3, int>;
	using vector3u = vector<3, unsigned int>;
	using vector4f = vector<4, float>;
	using vector4d = vector<4, double>;
	using vector4i = vector<4, int>;
	using vector4u = vector<4, unsigned int>;
	using color_rgb = vector<3, unsigned char>;
	using color_rgba = vector<4, unsigned char>;
	using colorf_rgb = vector3f;
	using colorf_rgba = vector4f;

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::identity()
	{
		matrix temp;
		for (std::size_t i = 0; i < Rows; i++)
			temp(i, i) = T(1);
		return temp;
	}

	template<std::size_t Rows, std::size_t Columns, typename T>	
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::translate(const vector<2, T>& position)
	{
		return matrix(
			1.0f, 0.0f, position.x(),
			0.0f, 1.0f, position.y(),
			0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::scale(const accel::size<2, T>& value)
	{
		return matrix(
			value.x(), 0.0f, 0.0f,
			0.0f, value.y(), 0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::rotate(const angle<T>& value)
	{
		return matrix(
			std::cos(value), std::sin(value), 0.0f,
			-std::sin(value), std::cos(value), 0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::shear(const vector<2, T>& value)
	{
		return matrix(
			1.0f, value.x(), 0.0f,
			value.y(), 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::translate(const vector<3, T>& position)
	{
		return matrix(
			1.0f, 0.0f, 0.0f, position.x(),
			0.0f, 1.0f, 0.0f, position.y(),
			0.0f, 0.0f, 1.0f, position.z(),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::scale(const vector<3, T>& position)
	{
		return matrix(
			position.x(), 0.0f, 0.0f, 0.0f,
			0.0f, position.y(), 0.0f, 0.0f,
			0.0f, 0.0f, position.z(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::rotate_x(const angle<T>& value)
	{
		return matrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, std::cos(value), -std::sin(value), 0.0f,
			0.0f, std::sin(value), std::cos(value), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::rotate_y(const angle<T>& value)
	{
		return matrix(
			std::cos(value), 0.0f, std::sin(value), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-std::sin(value), 0.0f, std::cos(value), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::rotate_z(const angle<T>& value)
	{
		return matrix(
			std::cos(value), -std::sin(value), 0, 0,
			std::sin(value), std::cos(value), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::perspective(const angle<T>& horizontal_fov, T aspect_ratio, T near_z, T far_z)
	{
		auto vertical_fov = angle<T>::atan((horizontal_fov / T(2)).tan() / aspect_ratio) * T(2);
		return perspective_v(vertical_fov, aspect_ratio, near_z, far_z);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::perspective_v(const angle<T>& vertical_fov, T aspect_ratio, T near_z, T far_z)
	{
		T tan_half_angle = (vertical_fov / T(2)).tan();
		T negative_range = near_z - far_z;
		return matrix(
			T(1) / (aspect_ratio * tan_half_angle), 0, 0, 0,
			0, T(1) / tan_half_angle, 0, 0,
			0, 0, (far_z + near_z) / negative_range, (T(2) * far_z * near_z) / negative_range,
			0, 0, -1, 0
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::lookat(const point<3, T>& target, const point<3, T>& at, const vector<3, T>& up)
	{
		vector<3, T> z_axis = target.vector_to(at).normalized();
		vector<3, T> x_axis = (up ^ z_axis).normalized();
		vector<3, T> y_axis = z_axis ^ x_axis;
		const vector<3, T>& target_vector = target;
		return matrix(
			x_axis.x(), x_axis.y(), x_axis.z(), -(x_axis * target_vector),
			y_axis.x(), y_axis.y(), y_axis.z(), -(y_axis * target_vector),
			z_axis.x(), z_axis.y(), z_axis.z(), -(z_axis * target_vector),
			0, 0, 0, 1
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::orthographic(const rectangle<T>& rect, T z_near, T z_far)
	{
		return matrix(
			T(2) / (rect.right() - rect.left()), 0, 0, -((rect.right() + rect.left()) / (rect.right() - rect.left())),
			0, T(2) / (rect.top() - rect.bottom()), 0, -((rect.top() + rect.bottom()) / (rect.top() - rect.bottom())),
			0, 0, T(-2) / (z_far - z_near), -((z_far + z_near) / (z_far - z_near)),
			0, 0, 0, 1
		);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<typename ...Ts>
	inline constexpr matrix<Rows, Columns, T>::matrix(Ts ...values) : m_data{ static_cast<T>(values)... } 
	{
		static_assert(sizeof...(values) <= (Rows * Columns), "Too many values in constructor.");
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr matrix<Rows, Columns, T>::matrix(const std::array<T, Rows * Columns>& data) : m_data(data) {}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr matrix<Rows, Columns, T>::matrix(std::array<T, Rows * Columns>&& data) : m_data(std::move(data)) {}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr std::size_t matrix<Rows, Columns, T>::rows() { return Rows; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr std::size_t matrix<Rows, Columns, T>::columns() { return Columns; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr std::size_t matrix<Rows, Columns, T>::size() { return Rows * Columns; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr typename matrix<Rows, Columns, T>::iterator matrix<Rows, Columns, T>::begin() { return m_data.begin(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr typename matrix<Rows, Columns, T>::iterator matrix<Rows, Columns, T>::end() { return m_data.end(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr typename matrix<Rows, Columns, T>::const_iterator matrix<Rows, Columns, T>::cbegin() const { return m_data.cbegin(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr typename matrix<Rows, Columns, T>::const_iterator matrix<Rows, Columns, T>::cend() const { return m_data.cend(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr vector<Columns, T> matrix<Rows, Columns, T>::row(std::size_t index) const
	{
		return vector<Columns, T>(m_data[index]);
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr vector<Rows, T> matrix<Rows, Columns, T>::column(std::size_t index) const
	{
		vector<Rows, T> column;
		for (std::size_t i = 0; i < Rows; i++)
			column[i] = m_data[i][index];
		return column;
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr matrix<Rows, Columns, T> matrix<Rows, Columns, T>::transposed() const
	{
		matrix result;
		for (std::size_t row = 0; row < Rows; row++)
		{
			for (std::size_t column = 0; column < Columns; column++)
				result.m_data[column][row] = m_data[row][column];
		}
		return result;
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr bool matrix<Rows, Columns, T>::operator==(const matrix<Rows, Columns, T>& other) const { return m_data == other.m_data; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr bool matrix<Rows, Columns, T>::operator!=(const matrix& other) const { return !operator==(other.m_data); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	template<std::size_t N>
	inline constexpr matrix<Rows, N, T> matrix<Rows, Columns, T>::operator*(const matrix<Columns, N, T>& other) const
	{
		matrix<Rows, N, T> result;
		for (std::size_t row = 0; row < Rows; row++)
		{
			for (std::size_t column = 0; column < N; column++)
			{
				for (std::size_t inner = 0; inner < Columns; inner++)
					result(row, column) += m_data[row][inner] * other(inner, column);
			}
		}
		return result;
	}

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr const T& matrix<Rows, Columns, T>::at(std::size_t row, std::size_t column) const { return m_data.at(row).at(column); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T& matrix<Rows, Columns, T>::at(std::size_t row, std::size_t column) { return m_data.at(row).at(column); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr const T& matrix<Rows, Columns, T>::at(std::size_t index) const { return m_data.at(index); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T& matrix<Rows, Columns, T>::at(std::size_t index) { return m_data.at(index); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr const T* matrix<Rows, Columns, T>::data() const { return m_data.data(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T* matrix<Rows, Columns, T>::data() { return m_data.data(); }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T matrix<Rows, Columns, T>::operator()(std::size_t row, std::size_t column) const { return m_data[row][column]; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T& matrix<Rows, Columns, T>::operator()(std::size_t row, std::size_t column) { return m_data[row][column]; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T matrix<Rows, Columns, T>::operator()(std::size_t index) const { return m_data[index]; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	inline constexpr T& matrix<Rows, Columns, T>::operator()(std::size_t index) { return m_data[index]; }

	template<std::size_t Rows, std::size_t Columns, typename T>
	std::ostream& operator<<(std::ostream& stream, const matrix<Rows, Columns, T>& m)
	{
		if constexpr (Rows == Columns)
			stream << "mat" << Rows << "(";
		else
			stream << "mat" << Rows << "x" << Columns << "(";
		for (std::size_t row = 0; row < Rows; row++)
		{
			stream << "(";
			for (std::size_t column = 0; column < Columns; column++)
				stream << m(row, column) << (column == Columns - 1 ? "" : ", ");
			stream << (row == Rows - 1 ? ")" : "), ");
		}
		stream << ")";
		return stream;
	}
}

#endif