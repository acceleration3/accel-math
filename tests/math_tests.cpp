#include <iostream>
#include <type_traits>

#include <cassert>

#include <accel/math.hpp>

using namespace accel;

int main(int argc, char* argv[])
{
	// ----------------------------------------------------
	// Rectangle
	// ----------------------------------------------------

	{
		rectanglef rect(point2f(100.0f, 100.0f), size2f(100.0f, 100.0f));
		assert(rect.top() == 100.0f);
		assert(rect.left() == 100.0f);
		assert(rect.bottom() == 200.0f);
		assert(rect.right() == 200.0f);
		assert(rect.size() == size2f(100.0f, 100.0f));
		assert(rect.top_left() == point2f(100.0f, 100.0f));
		assert(rect.top_right() == point2f(200.0f, 100.0f));
		assert(rect.bottom_left() == point2f(100.0f, 200.0f));
		assert(rect.bottom_right() == point2f(200.0f, 200.0f));
		{
			auto rect2 = rect;
			rect2.offset(size2f(50.0f, 50.0f));
			assert(rect2 == rectanglef(150.0f, 150.0f, 250.0f, 250.0f));
			assert(rect.intersection(rect2) == rectanglef(150.0f, 150.0f, 200.0f, 200.0f));
		}
		{
			auto rect2 = rect;
			rect.pad(size2f(20.0f, 40.0f));
			assert(rect == rectanglef(60.0f, 80.0f, 240.0f, 220.0f));
		}
	}


	// ----------------------------------------------------
	// Angle
	// ----------------------------------------------------

	{
		radiansf angle = radiansf::asin(0.0f);
		assert(angle == 0.0f);
		assert(angle.sin() == 0.0f);
		assert(angle.cos() == 1.0f);
		assert(angle.tan() == 0.0f);
		angle += degreesf(180.0f);
		assert(angle == radiansf::pi());
	}


	// ----------------------------------------------------
	// Vector tests
	// ----------------------------------------------------

	// Constructors
	{
		{
			vector2f v;
			assert(v.x() == 0.0f);
			assert(v.y() == 0.0f);
		}

		{
			vector2f v(2.0f);
			assert(v.x() == 2.0f);
			assert(v.y() == 2.0f);
		}

		{
			vector2f v(2.0f, 5.0f);
			assert(v.x() == 2.0f);
			assert(v.y() == 5.0f);
		}

		{
			vector3f v(1.0f, 2.0f, 3.0f);
			assert(v.x() == 1.0f);
			assert(v.y() == 2.0f);
			assert(v.z() == 3.0f);
		}

		{
			vector2f base(3.0f, 2.0f);
			vector3f v(base, 1.0f);
			assert(v.x() == 3.0f);
			assert(v.y() == 2.0f);
			assert(v.z() == 1.0f);
		}
	}

	// Methods
	{
		vector2f v(6.0f, 7.0f);
		assert(v.mean() == 6.5f);
		assert(v.sum() == 13.0f);
		assert(v.length_squared() == 85.0f);
		assert(v.length() == std::sqrt(85.0f));
		assert(vector2f().length() == 0.0f); // Division by 0 case

		vector2f v1(1.0f, 0.0f);
		vector2f v2(0.0f, 1.0f);
		assert(v1.angle(v2) == degreesf(90.0f));
	}

	// Swizzle
	{
		vector2f v(2.0f, 3.0f);
		assert((v.swizzle<swizzle_x, swizzle_x>()) == vector2f(2.0f, 2.0f));
		assert((v.swizzle<swizzle_y, swizzle_y>()) == vector2f(3.0f, 3.0f));
		assert((v.swizzle<swizzle_y, swizzle_x>()) == vector2f(3.0f, 2.0f));
		assert((v.swizzle<swizzle_zero, swizzle_one>()) == vector2f(0.0f, 1.0f));		
		assert((v.swizzle<swizzle_zero, swizzle_one, swizzle_x, swizzle_y>()) == vector4f(0.0f, 1.0f, 2.0f, 3.0f));
	}
	
	// Operators
	{
		vector2f v1(4.0f, 5.0f);
		vector2f v2(3.0f, 2.0f);
		vector2f v3;
		assert(v1 + v2 == vector2f(7.0f, 7.0f));
		assert(v1 - v2 == vector2f(1.0f, 3.0f));
		assert(v1 * v2 == 22.0f);
		assert((v1 ^ v2) == -7.0f);
		assert((v2 * 2.0f) == vector2f(6.0f, 4.0f));
		assert((v2 / 2.0f) == vector2f(1.5f, 1.0f));

		{
			matrix3f m(
				1.0f, 2.0f, 3.0f,
				4.0f, 5.0f, 6.0f,
				7.0f, 8.0f, 9.0f
			);
			vector3f v(1.0f, 2.0f, 3.0f);
			assert((v * m) == vector3f(14.0f, 32.0f, 50.0f));
		}
	}


	// ----------------------------------------------------
	// Matrix tests
	// ----------------------------------------------------
	
	// Constructors
	{
		matrix2f m;
		assert(m == matrix2f(
			0.0f, 0.0f,
			0.0f, 0.0f
		));
	}

	// Useful matrices
	{
		assert(matrix2f::identity() == matrix2f(
			1.0f, 0.0f,
			0.0f, 1.0f
		));

		assert(matrix3f::identity() == matrix3f(
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f)
		);

		assert(matrix4f::identity() == matrix4f(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		assert(matrix4f::translate(vector3f(2.0f, 3.0f, 4.0f)) == matrix4f(
			1.0f, 0.0f, 0.0f, 2.0f,
			0.0f, 1.0f, 0.0f, 3.0f,
			0.0f, 0.0f, 1.0f, 4.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		/*
		auto proj = matrix4f::perspective(degreesf(90.0f), 16.0f / 9.0f, 1.0f, 100.0f);
		auto view = matrix4f::lookat(point3f(0.0f, 0.0f, 15.0f), point3f(0.0f, 0.0f, 0.0f), vector3f(0.0f, 1.0f, 0.0f));
		*/

		auto proj = matrix4f::orthographic(rectanglef(0.0f, 0.0f, 600.0f, 800.0f), 0.1f, 100.0f);
		std::cout << "Ortho: " << proj << "\n";
	}

	// Methods
	{
		{
			auto m = matrix3f::identity();
			assert(m.transposed() == m);
		}

		{
			auto m = matrix3f(
				1.0f, 2.0f, 3.0f,
				4.0f, 5.0f, 6.0f,
				7.0f, 8.0f, 9.0f
			);
			assert(m.transposed() == matrix3f(
				1.0f, 4.0f, 7.0f,
				2.0f, 5.0f, 8.0f,
				3.0f, 6.0f, 9.0f
			));
			assert(m.row(0) == vector3f(1.0f, 2.0f, 3.0f));
			assert(m.column(0) == vector3f(1.0f, 4.0f, 7.0f));
		}
	}

	// Multiplication
	{
		{
			matrix<3, 2> m1(
				1.0f, 2.0f, 3.0f,
				4.0f, 5.0f, 6.0f
			);
			matrix<2, 3> m2(
				1.0f, 2.0f,
				3.0f, 4.0f,
				5.0f, 6.0f
			);
			assert((m1 * m2) == matrix3f(
				9.0f, 12.0f, 15.0f,
				19.0f, 26.0f, 33.0f,
				29.0f, 40.0f, 51.0f
			));
		}

		{
			matrix<2, 3> m1(
				0.0f, 4.0f, -2.0f,
				-4.0f, -3.0f, 0.0f
			);
			matrix<3, 2> m2(
				0.0f, 1.0f,
				1.0f, -1.0f,
				2.0f, 3.0f
			);
			assert((m1 * m2) == matrix2f(
				0.0f, -10.0f,
				-3.0f, -1.0f
			));
		}
	}

	std::cout << "All tests completed successfully.\n";
	
	return 0;
}
