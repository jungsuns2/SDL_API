#include "pch.h"
#include "Collision.h"

#include "Entity/Entity.h"
#include "CollisionMath.h"

Collision& Collision::Get()
{
	static Collision collision{};
	return collision;
}

bool Collision::IsCollisionEnter(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = GetCollidedEntityPair(entity0, entity1);

	if (const auto& foundPair = std::find(mCollidedEntityPairs.begin(), 
		mCollidedEntityPairs.end(), 
		collidedEntityPair);
		foundPair != mCollidedEntityPairs.end())
	{
		const auto& foundPreviousPair = 
			std::find(mPreviousCollidedEntityPairs.begin(), 
				mPreviousCollidedEntityPairs.end(), 
				collidedEntityPair);
		
		return foundPreviousPair == mPreviousCollidedEntityPairs.end();
	}

	return false;
}

bool Collision::IsCollisionStay(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = GetCollidedEntityPair(entity0, entity1);

	const auto& foundCollidedEntityPair =
		std::find(mCollidedEntityPairs.begin(),
			mCollidedEntityPairs.end(), collidedEntityPair);

	return foundCollidedEntityPair != mCollidedEntityPairs.end();
}

bool Collision::IsCollisionExit(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = Collision::Get().GetCollidedEntityPair(entity0, entity1);

	if (const auto& foundPreviousCollidedEntityPair =
		std::find(mPreviousCollidedEntityPairs.begin(),
			mPreviousCollidedEntityPairs.end(),
			collidedEntityPair);
		foundPreviousCollidedEntityPair != mPreviousCollidedEntityPairs.end())
	{
		const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(),
			mCollidedEntityPairs.end(), 
			collidedEntityPair);

		return foundCollidedEntityPair == mCollidedEntityPairs.end();
	}

	return false;
}

std::pair<const Entity*, const Entity*> Collision::GetCollidedEntityPair(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair{};
	if (&entity0 < &entity1)
	{
		collidedEntityPair = { &entity0, &entity1 };
	}
	else
	{
		collidedEntityPair = { &entity1, &entity0 };
	}

	return collidedEntityPair;
}

Quad Collision::ConvertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
{
	const Point position = transform.position + boxCollider.offset;
	const Scale boxHalfSize = transform.scale * boxCollider.size * 0.5f;

	const Quad local =
	{
		.leftTop = { -boxHalfSize.width, -boxHalfSize.height },
		.rightTop = { boxHalfSize.width, -boxHalfSize.height },
		.leftBottom = { -boxHalfSize.width, boxHalfSize.height },
		.rightBottom = { boxHalfSize.width, boxHalfSize.height }
	};

	const Quad rotate =
	{
		.leftTop = Math::RotatePoint(local.leftTop, -transform.angle),
		.rightTop = Math::RotatePoint(local.rightTop, -transform.angle),
		.leftBottom = Math::RotatePoint(local.leftBottom, -transform.angle),
		.rightBottom = Math::RotatePoint(local.rightBottom, -transform.angle)
	};

	const Quad result
	{
		.leftTop = {.x = position.x + rotate.leftTop.x, .y = position.y - rotate.leftTop.y },
		.rightTop = {.x = position.x + rotate.rightTop.x, .y = position.y - rotate.rightTop.y },
		.leftBottom = {.x = position.x + rotate.leftBottom.x, .y = position.y - rotate.leftBottom.y },
		.rightBottom = {.x = position.x + rotate.rightBottom.x, .y = position.y - rotate.rightBottom.y }
	};

	return result;
}

Circle Collision::ConvertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const
{
	const Circle result =
	{
		.center = transform.position + circleCollider.offset,
		.radius = transform.scale.width * circleCollider.radius
	};

	return result;
}

Line Collision::ConvertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const
{
	const Point position = transform.position + lineCollider.offset;
	const Scale boxHalfSize = transform.scale * lineCollider.scale * 0.5f;

	const Rect rect
	{
		.left = position.x - boxHalfSize.width,
		.top = position.y + boxHalfSize.height,
		.right = position.x + boxHalfSize.width,
		.bottom = position.y - boxHalfSize.height,
	};

	float centerY = rect.top + (transform.scale.height * lineCollider.scale.height * 0.5f);

	const Line result =
	{
		.point0 = {.x = rect.left, .y = rect.top },
		.point1 = {.x = rect.right, .y = rect.bottom }
	};

	return result;
}

void Collision::RegisterCollidedEntityPairs(const Entity& entity0, const Entity& entity1) 
{
	std::pair<const Entity*, const Entity*> colliderEntityPair = GetCollidedEntityPair(entity0, entity1);

	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), colliderEntityPair);
		foundCollidedEntityPair == mCollidedEntityPairs.end())
	{
		mCollidedEntityPairs.push_back(colliderEntityPair);
	}
}

const std::vector<std::pair<const Entity*, const Entity*>>& Collision::GetCollidedEntityPairs() const
{
	return mCollidedEntityPairs;
}

const std::vector<std::pair<const Entity*, const Entity*>>& Collision::GetPreviousCollidedEntityPairs() const
{
	return mPreviousCollidedEntityPairs;
}

void Collision::UpdateEntityPairs()
{
	mPreviousCollidedEntityPairs = mCollidedEntityPairs;
	mCollidedEntityPairs.clear();
}

bool Collision::CheckCollisionBoxBox(const Entity& entity0, const Entity& entity1)
{
	if (not entity0.HasComponent<BoxCollider>()
		or not entity1.HasComponent<BoxCollider>())
	{
		return false;
	}

	const Transform* transform0 = entity0.GetComponent<Transform>();
	const BoxCollider* boxCollider0 = entity0.GetComponent<BoxCollider>();
	const Quad quad0 = ConvertBoxColliderToWorldBox(*transform0, *boxCollider0);

	const Transform* transform1 = entity1.GetComponent<Transform>();
	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
	const Quad quad1 = ConvertBoxColliderToWorldBox(*transform1, *boxCollider1);

	if (CollisionMath::IsCollidedSqureWithSqure(quad0, quad1))
	{
		RegisterCollidedEntityPairs(entity0, entity1);
		return true;
	}

	return false;
}

bool Collision::CheckCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity)
{
	if (not boxEntity.HasComponent<BoxCollider>()
		or not circleEntity.HasComponent<CircleCollider>())
	{
		return false;
	}

	// TODO: Quad를 사용해서 수정하기
	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	//const std::array<Point, 5> points = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
	const Circle circle = ConvertCircleColliderToWorldCircle(*circleTransform, *circleCollider);

	//if (Collision::IsCollidedSqureWithCircle(rect, circle))
	//{
	//	registerCollidedEntityPairs(boxEntity, circleEntity);
	//	return true;
	//}

	return false;
}

bool Collision::CheckCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity)
{
	if (not boxEntity.HasComponent<BoxCollider>()
		or not lineEntity.HasComponent<LineCollider>())
	{
		return false;
	}

	// TODO: Quad를 사용해서 수정하기
	//const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	//const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	//const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	//const Transform* lineTransform = lineEntity.GetComponent<Transform>();
	//const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
	//const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);

	//if (Collision::IsCollidedSqureWithLine(rect, line))
	//{
	//	registerCollidedEntityPairs(boxEntity, lineEntity);
	//	return true;
	//}

	return false;
}

bool Collision::CheckCollisionCircleCircle(const Entity& entity0, const Entity& entity1)
{
	if (not entity0.HasComponent<CircleCollider>()
		or not entity1.HasComponent<CircleCollider>())
	{
		return false;
	}

	const Transform* Transform0 = entity0.GetComponent<Transform>();
	const CircleCollider* Collider0 = entity0.GetComponent<CircleCollider>();
	const Circle circle0 = ConvertCircleColliderToWorldCircle(*Transform0, *Collider0);

	const Transform* Transform1 = entity1.GetComponent<Transform>();
	const CircleCollider* Collider1 = entity1.GetComponent<CircleCollider>();
	const Circle circle1 = ConvertCircleColliderToWorldCircle(*Transform1, *Collider1);

	if (CollisionMath::IsCollidedCircleWithCircle(circle0, circle1))
	{
		RegisterCollidedEntityPairs(entity0, entity1);
		return true;
	}

	return false;
}

bool Collision::CheckCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity)
{
	if (not circleEntity.HasComponent<CircleCollider>()
		or not lineEntity.HasComponent<LineCollider>())
	{
		return false;
	}

	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
	const Circle circle = ConvertCircleColliderToWorldCircle(*circleTransform, *circleCollider);

	const Transform* lineTransform = lineEntity.GetComponent<Transform>();
	const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
	const Line line = ConvertLineColliderToWorldLine(*lineTransform, *lineCollider);

	if (CollisionMath::IsCollidedCircleWithLine(circle, line))
	{
		RegisterCollidedEntityPairs(circleEntity, lineEntity);
		return true;
	}

	return false;
}