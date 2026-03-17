#include "pch.h"
#include "Core/Helper.h"
#include "MainScene.h"

void MainScene::Initialize()
{
	mFont.Initilize("Resource/DroidSans.TTF");

	mLabel.SetFont(&mFont);
	mLabel.SetText(GetHelper()->GetRenderer(), "aaaaaaaaaaaaaaaaaaaaa");
	mLabel.SetPosition({ .x = 50.0f, .y = 10.0f });
	mLabel.SetSize(30);

}

bool MainScene::Update(const float deltaTime)
{
	mTextDstFRect =
	{
		.x = mLabel.GetPosition().x,
		.y = mLabel.GetPosition().y,
		.w = float(mLabel.GetWidth()),
		.h = float(mLabel.GetHeight())
	};

	SDL_Renderer* renderer = GetHelper()->GetRenderer();
	SDL_RenderCopyF(renderer, mLabel.GetTexture(), nullptr, &mTextDstFRect);	// 텍스트를 출력한다.

	return true;
}

void MainScene::Finalize()
{
	mFont.Finalize();
	mLabel.Finalize();
}
