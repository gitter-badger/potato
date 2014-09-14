#pragma once

#include "common.h"

#include "base.h"
#include "flash.h"

namespace c4g{
namespace core{

class IScene;

class IRender
{
public:
  virtual ~IRender() { ; }

public:
  virtual void Start() = 0;
  virtual bool Resize(const int& riWidth, const int& riHeight) = 0;
  virtual bool Render(const float& rfDeltaTime, IScene* const& rpScene) = 0;
  virtual void End() = 0;

public:
  virtual unsigned int GenerateTexId(const int& riWidth, const int& riHeight, const unsigned char* const& rpBuffer) = 0;
  virtual void DeleteTexId(const int& riCount, const unsigned int* const& rpiTexId) = 0;
};

}

namespace render{

class ITransform
{
public:
  virtual ~ITransform() { ; }

public:
  virtual void Translate(const float& rfX, const float& rfY, const float& rfZ = 0.0f) = 0;
  virtual void Scale(const float& rfX, const float& rfY, const float& rfZ = 1.0f) = 0;
  virtual void Rotate(const float& rfAngle, const float& rfX, const float& rfY, const float& rfZ, const float& rfAX = 0.0f, const float& rfAY = 0.0f, const float& rfAZ = 0.0f) = 0;
  /// must have four vertex, means the array have 4 * 3 float
  /// and the indices is { 0, 1, 2, 2, 1, 3 };
  virtual void Free(float* const& rpfData) = 0;
};

class ICanvas
{
public:
  virtual ~ICanvas() { ; }

public:
  virtual void EffectBegin(flash::IEffect* const& rpEffect) = 0;
  virtual void EffectEnd(flash::IEffect* const& rpEffect) = 0;
  virtual void DrawGlyph(const base::Glyph& rGlyph, flash::IEffect* const& rpEffect = NULL) = 0;
  virtual void DrawGlyph(const base::Glyph& rGlyph, const float& rfWidth, const float& rfHeight, flash::IEffect* const& rpEffect = NULL) = 0;
};

}
}

FUNC_API_DECLARE(CreateRender, c4g::core::IRender, const c4g::base::Config);
FUNC_API_DECLARE(DestroyRender, c4g::core::IRender, const c4g::base::Config);
