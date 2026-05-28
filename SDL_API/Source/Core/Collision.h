#pragma once

namespace Collision
{
	inline bool IsCollidedSqureWithPoint(const Rect rect, const Point point);
	inline bool IsCollidedSqureWithPoint(const std::array<Point, 5> points, const Point point);
	inline bool IsCollidedSqureWithSqure(const std::array<Point, 5> lhs, const std::array<Point, 5> rhs);
	inline bool IsCollidedSqureWithLine(const Rect rect, const Line line);
	inline bool IsCollidedSqureWithCircle(const Rect rect, const Circle& circle);
	inline bool IsCollidedCircleWithPoint(const Circle& circle, const Point point);
	inline bool IsCollidedCircleWithLine(const Circle& circle, const Line line);
	inline bool IsCollidedCircleWithCircle(const Circle lhs, const Circle rhs);
	inline bool DoLinesIntersect(Line line0, Line line1);

	bool IsCollidedSqureWithPoint(const Rect rect, const Point point)
	{
		const bool result = rect.left <= point.x and point.x <= rect.right
			and rect.bottom <= point.y and point.y <= rect.top;

		return result;
	}

	bool IsCollidedSqureWithPoint(const std::array<Point, 5>points, const Point point)
	{
		const float cp1 = Math::CrossProduct2D(points[0], points[1], point);
		const float cp2 = Math::CrossProduct2D(points[1], points[2], point);
		const float cp3 = Math::CrossProduct2D(points[2], points[3], point);
		const float cp4 = Math::CrossProduct2D(points[3], points[4], point);
		
		const bool allPositive = (cp1 >= 0 and cp2 >= 0 and cp3 >= 0 and cp4 >= 0);
		const bool allNegative = (cp1 <= 0 and cp2 <= 0 and cp3 <= 0 and cp4 <= 0);

		return allPositive || allNegative;
	}

	bool IsCollidedSqureWithSqure(const std::array<Point, 5> lhs, const std::array<Point, 5> rhs)
	{
		const bool rhsInLhs = IsCollidedSqureWithPoint(lhs, rhs[0]) // 왼쪽 위
			or IsCollidedSqureWithPoint(lhs, rhs[1]) // 오른쪽 위
			or IsCollidedSqureWithPoint(lhs, rhs[2]) // 오른쪽 아래 
			or IsCollidedSqureWithPoint(lhs, rhs[3]); // 왼쪽 아래

		const bool lhsInRhs = IsCollidedSqureWithPoint(rhs, lhs[0]) 
			or IsCollidedSqureWithPoint(rhs, lhs[1]) 
			or IsCollidedSqureWithPoint(rhs, lhs[2]) 
			or IsCollidedSqureWithPoint(rhs, lhs[3]);

		return rhsInLhs or lhsInRhs;
	}

	bool IsCollidedSqureWithLine(const Rect rect, const Line line)
	{
		const Line leftLine =
		{
			.point0 = {.x = rect.left, .y = rect.top },
			.point1 = {.x = rect.left, .y = rect.bottom }
		};

		const Line rightLine =
		{
			.point0 = {.x = rect.right, .y = rect.top },
			.point1 = {.x = rect.right, .y = rect.bottom }
		};

		const Line topLine =
		{
			.point0 = {.x = rect.left, .y = rect.top },
			.point1 = {.x = rect.right, .y = rect.top }
		};

		const Line bottomLine =
		{
			.point0 = {.x = rect.left, .y = rect.bottom },
			.point1 = {.x = rect.right, .y = rect.bottom }
		};

		const bool result = DoLinesIntersect(leftLine, line)
			or DoLinesIntersect(rightLine, line)
			or DoLinesIntersect(topLine, line)
			or DoLinesIntersect(bottomLine, line);

		return result;
	}

	bool IsCollidedSqureWithCircle(const Rect rect, const Circle& circle)
	{
		Line leftLine =
		{
			.point0 = {.x = rect.left, .y = rect.top },
			.point1 = {.x = rect.left, .y = rect.bottom },
		};

		Line topLine =
		{
			.point0 = {.x = rect.left, .y = rect.top },
			.point1 = {.x = rect.right, .y = rect.top },
		};

		Line rightLine =
		{
			.point0 = {.x = rect.right, .y = rect.top },
			.point1 = {.x = rect.right, .y = rect.bottom },
		};

		Line bottomLine =
		{
			.point0 = {.x = rect.left, .y = rect.bottom },
			.point1 = {.x = rect.right, .y = rect.bottom },
		};

		const bool result = IsCollidedCircleWithLine(circle, leftLine)
			or IsCollidedCircleWithLine(circle, topLine)
			or IsCollidedCircleWithLine(circle, rightLine)
			or IsCollidedCircleWithLine(circle, bottomLine);

		return result;
	}

	bool IsCollidedCircleWithPoint(const Circle& circle, const Point point)
	{
		const Point centerToPoint = circle.center - point;
		const float centerToPointLength = Math::GetVectorLength(centerToPoint);
		const bool result = centerToPointLength <= circle.radius;

		return result;
	}

	bool IsCollidedCircleWithLine(const Circle& circle, const Line line)
	{
		const Point ac = circle.center - line.point0;
		const Point ab = line.point1 - line.point0;

		const Point abDirection = Math::NormalizeVector(ab);
		float t = Math::DotProduct2D(ac, abDirection);

		const float abLength = Math::GetVectorLength(ab);
		t = std::clamp(t, 0.0f, abLength);

		const Point dPoint =
		{
			line.point0.x + abDirection.x * t,
			line.point0.y + abDirection.y * t
		};

		const Point diff = circle.center - dPoint;

		bool result = circle.radius >= Math::GetVectorLength(diff);

		return result;
	}

	bool IsCollidedCircleWithCircle(const Circle lhs, const Circle rhs)
	{
		const float radius = lhs.radius + rhs.radius;

		const Point toTarget = lhs.center - rhs.center;
		const float distance = Math::GetVectorLength(toTarget);

		const bool result = radius >= distance;

		return result;
	}

	bool DoLinesIntersect(Line line0, Line line1)
	{
		const float ccwProductFromLine0 = Math::CrossProduct2D(line0.point0, line0.point1, line1.point0) * Math::CrossProduct2D(line0.point0, line0.point1, line1.point1);
		const float ccwProductFromLine1 = Math::CrossProduct2D(line1.point0, line1.point1, line0.point0) * Math::CrossProduct2D(line1.point0, line1.point1, line0.point1);
		bool bIntersecting = false;

		if (std::abs(ccwProductFromLine0) < FLT_EPSILON and std::abs(ccwProductFromLine1) < FLT_EPSILON)
		{
			if (std::make_pair(line0.point0.x, line0.point0.y) > std::make_pair(line0.point1.x, line0.point1.y))
			{
				std::swap(line0.point0, line0.point1);
			}

			if (std::make_pair(line1.point0.x, line1.point0.y) > std::make_pair(line1.point1.x, line1.point1.y))
			{
				std::swap(line1.point0, line1.point1);
			}

			bIntersecting = std::make_pair(line1.point0.x, line1.point0.y) <= std::make_pair(line0.point1.x, line0.point1.y)
				and std::make_pair(line0.point0.x, line0.point0.y) <= std::make_pair(line1.point1.x, line1.point1.y);
		}
		else
		{
			bIntersecting = ccwProductFromLine0 <= FLT_EPSILON && ccwProductFromLine1 <= FLT_EPSILON;
		}

		return bIntersecting;
	}
}