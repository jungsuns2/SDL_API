#pragma once

namespace Collision
{
	inline bool IsCollidedSqureWithPoint(const Rect rect, const Point point);
	inline bool IsCollidedSqureWithSqure(const Rect lhs, const Rect rhs);
	inline bool IsCollidedSqureWithLine(const Rect rect, const Line line);
	inline bool IsCollidedSqureWithCircle(const Rect rect, const Point center, const float radius);
	inline bool IsCollidedCircleWithPoint(const Point center, const float radius, const Point point);
	inline bool IsCollidedCircleWithLine(const Point center, const float radius, const Line line);
	inline bool IsCollidedCircleWithCircle(const Ellipse lhs, const Ellipse rhs);
	inline bool DoLinesIntersect(Line line0, Line line1);

	bool IsCollidedSqureWithPoint(const Rect rect, const Point point)
	{
		const bool result = rect.left <= point.x and point.x <= rect.right
			and rect.bottom <= point.y and point.y <= rect.top;

		return result;
	}

	bool IsCollidedSqureWithSqure(const Rect lhs, const Rect rhs)
	{
		const bool result = IsCollidedSqureWithPoint(lhs, { .x = float(rhs.left), .y = float(rhs.top) })
			or IsCollidedSqureWithPoint(lhs, { .x = float(rhs.left), .y = float(rhs.bottom) })
			or IsCollidedSqureWithPoint(lhs, { .x = float(rhs.right), .y = float(rhs.top) })
			or IsCollidedSqureWithPoint(lhs, { .x = float(rhs.right), .y = float(rhs.bottom) });

		return result;
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

	bool IsCollidedSqureWithCircle(const Rect rect, const Point center, const float radius)
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

		const bool result = IsCollidedCircleWithLine(center, radius, leftLine)
			and IsCollidedCircleWithLine(center, radius, topLine)
			and IsCollidedCircleWithLine(center, radius, rightLine)
			and IsCollidedCircleWithLine(center, radius, bottomLine);

		return result;
	}

	bool IsCollidedCircleWithPoint(const Point center, const float radius, const Point point)
	{
		const Point centerToPoint = center - point;
		const float centerToPointLength = Math::GetVectorLength(centerToPoint);
		const bool result = centerToPointLength <= radius;

		return result;
	}

	bool IsCollidedCircleWithLine(const Point center, const float radius, const Line line)
	{
		const Point ac = center - line.point0;
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

		const Point diff = center - dPoint;

		bool result = radius >= Math::GetVectorLength(diff);

		return result;
	}

	bool IsCollidedCircleWithCircle(const Ellipse lhs, const Ellipse rhs)
	{
		const float radius = lhs.radiusX + rhs.radiusX;

		const Point toTarget = lhs.point - rhs.point;
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