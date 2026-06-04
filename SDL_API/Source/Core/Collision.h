#pragma once

namespace Collision
{
	inline bool IsCollidedRectWithPoint(const Rect rect, const Point point);
	inline bool IsCollidedQuadWithPoint(const Quad& quad, const Point point);
	inline bool IsCollidedRectWithRect(const Rect lhs, const Rect rhs);
	inline bool IsCollidedSqureWithSqure(const Quad& lhs, const Quad& rhs);
	inline bool IsCollidedSqureWithLine(const Rect rect, const Line line);
	inline bool IsCollidedSqureWithCircle(const Rect rect, const Circle& circle);
	inline bool IsCollidedCircleWithPoint(const Circle& circle, const Point point);
	inline bool IsCollidedCircleWithLine(const Circle& circle, const Line line);
	inline bool IsCollidedCircleWithCircle(const Circle lhs, const Circle rhs);
	inline bool DoLinesIntersect(Line line0, Line line1);

	bool IsCollidedRectWithPoint(const Rect rect, const Point point)
	{
		const bool result = rect.left <= point.x and point.x <= rect.right
			and rect.bottom <= point.y and point.y <= rect.top;

		return result;
	}

	bool IsCollidedQuadWithPoint(const Quad& quad, const Point point)
	{
		const float cross0 = Math::CrossProduct2D(quad.leftTop, quad.rightTop, point);
		const float cross1 = Math::CrossProduct2D(quad.rightTop, quad.rightBottom, point);
		const float cross2 = Math::CrossProduct2D(quad.rightBottom, quad.leftBottom, point);
		const float cross3 = Math::CrossProduct2D(quad.leftBottom, quad.leftTop, point);

		const bool result0 = (cross0 >= 0 and cross1 >= 0 and cross2 >= 0 and cross3 >= 0);
		const bool result1 = (cross0 <= 0 and cross1 <= 0 and cross2 <= 0 and cross3 <= 0);

		return result0 or result1;
	}

	bool IsCollidedRectWithRect(const Rect lhs, const Rect rhs)
	{
		const bool result = IsCollidedRectWithPoint(lhs, { .x = float(rhs.left), .y = float(rhs.top) })
			or IsCollidedRectWithPoint(lhs, { .x = float(rhs.left), .y = float(rhs.bottom) })
			or IsCollidedRectWithPoint(lhs, { .x = float(rhs.right), .y = float(rhs.top) })
			or IsCollidedRectWithPoint(lhs, { .x = float(rhs.right), .y = float(rhs.bottom) });

		return result;
	}

	bool IsCollidedSqureWithSqure(const Quad& lhs, const Quad& rhs)
	{
		const bool lhsToRhs = IsCollidedQuadWithPoint(lhs, rhs.leftTop)
			or IsCollidedQuadWithPoint(lhs, rhs.rightTop)
			or IsCollidedQuadWithPoint(lhs, rhs.leftBottom)
			or IsCollidedQuadWithPoint(lhs, rhs.rightBottom);

		const bool rhsToLhs = IsCollidedQuadWithPoint(rhs, lhs.leftTop)
			or IsCollidedQuadWithPoint(rhs, lhs.rightTop)
			or IsCollidedQuadWithPoint(rhs, lhs.leftBottom)
			or IsCollidedQuadWithPoint(rhs, lhs.rightBottom);

		return lhsToRhs or rhsToLhs;
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