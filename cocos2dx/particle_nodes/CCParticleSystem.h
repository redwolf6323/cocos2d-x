/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CCPARTICLE_SYSTEM_H__
#define __CCPARTICLE_SYSTEM_H__

#include "CCProtocols.h"
#include "base_nodes/CCNode.h"
#include "cocoa/CCDictionary.h"
#include "cocoa/CCString.h"

NS_CC_BEGIN

/**
 * @addtogroup particle_nodes
 * @{
 */

class ParticleBatchNode;

//* @enum
enum {
    /** The Particle emitter lives forever */
    kParticleDurationInfinity = -1,

    /** The starting size of the particle is equal to the ending size */
    kParticleStartSizeEqualToEndSize = -1,

    /** The starting radius of the particle is equal to the ending radius */
    kParticleStartRadiusEqualToEndRadius = -1
};

//* @enum
enum {
    /** Gravity mode (A mode) */
    kParticleModeGravity,

    /** Radius mode (B mode) */
    kParticleModeRadius,    
};


/** @typedef tPositionType
possible types of particle positions
*/
typedef enum {
    /** Living particles are attached to the world and are unaffected by emitter repositioning. */
    kPositionTypeFree,

    /** Living particles are attached to the world but will follow the emitter repositioning.
    Use case: Attach an emitter to an sprite, and you want that the emitter follows the sprite.
    */
    kPositionTypeRelative,

    /** Living particles are attached to the emitter and are translated along with it. */
    kPositionTypeGrouped,
}tPositionType;


/**
Structure that contains the values of each particle
*/
typedef struct sParticle {
    Point     pos;
    Point     startPos;

    Color4F    color;
    Color4F    deltaColor;

    float        size;
    float        deltaSize;

    float        rotation;
    float        deltaRotation;

    float        timeToLive;

    unsigned int    atlasIndex;

    //! Mode A: gravity, direction, radial accel, tangential accel
    struct {
        Point        dir;
        float        radialAccel;
        float        tangentialAccel;
    } modeA;

    //! Mode B: radius mode
    struct {
        float        angle;
        float        degreesPerSecond;
        float        radius;
        float        deltaRadius;
    } modeB;

}tParticle;

//typedef void (*CC_UPDATE_PARTICLE_IMP)(id, SEL, tParticle*, Point);

class Texture2D;

/** @brief Particle System base class.
Attributes of a Particle System:
- emission rate of the particles
- Gravity Mode (Mode A):
- gravity
- direction
- speed +-  variance
- tangential acceleration +- variance
- radial acceleration +- variance
- Radius Mode (Mode B):
- startRadius +- variance
- endRadius +- variance
- rotate +- variance
- Properties common to all modes:
- life +- life variance
- start spin +- variance
- end spin +- variance
- start size +- variance
- end size +- variance
- start color +- variance
- end color +- variance
- life +- variance
- blending function
- texture

cocos2d also supports particles generated by Particle Designer (http://particledesigner.71squared.com/).
'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
cocos2d uses a another approach, but the results are almost identical. 

cocos2d supports all the variables used by Particle Designer plus a bit more:
- spinning particles (supported when using ParticleSystemQuad)
- tangential acceleration (Gravity mode)
- radial acceleration (Gravity mode)
- radius direction (Radius mode) (Particle Designer supports outwards to inwards direction only)

It is possible to customize any of the above mentioned properties in runtime. Example:

@code
emitter.radialAccel = 15;
emitter.startSpin = 0;
@endcode

*/
class CC_DLL ParticleSystem : public Node, public TextureProtocol
{    
protected:
    std::string _plistFile;
    //! time elapsed since the start of the system (in seconds)
    float _elapsed;

    // Different modes
    //! Mode A:Gravity + Tangential Accel + Radial Accel
    struct {
        /** Gravity value. Only available in 'Gravity' mode. */
        Point gravity;
        /** speed of each particle. Only available in 'Gravity' mode.  */
        float speed;
        /** speed variance of each particle. Only available in 'Gravity' mode. */
        float speedVar;
        /** tangential acceleration of each particle. Only available in 'Gravity' mode. */
        float tangentialAccel;
        /** tangential acceleration variance of each particle. Only available in 'Gravity' mode. */
        float tangentialAccelVar;
        /** radial acceleration of each particle. Only available in 'Gravity' mode. */
        float radialAccel;
        /** radial acceleration variance of each particle. Only available in 'Gravity' mode. */
        float radialAccelVar;
        /** set the rotation of each particle to its direction Only available in 'Gravity' mode. */
        bool rotationIsDir;
    } modeA;

    //! Mode B: circular movement (gravity, radial accel and tangential accel don't are not used in this mode)
    struct {
        /** The starting radius of the particles. Only available in 'Radius' mode. */
        float startRadius;
        /** The starting radius variance of the particles. Only available in 'Radius' mode. */
        float startRadiusVar;
        /** The ending radius of the particles. Only available in 'Radius' mode. */
        float endRadius;
        /** The ending radius variance of the particles. Only available in 'Radius' mode. */
        float endRadiusVar;            
        /** Number of degrees to rotate a particle around the source pos per second. Only available in 'Radius' mode. */
        float rotatePerSecond;
        /** Variance in degrees for rotatePerSecond. Only available in 'Radius' mode. */
        float rotatePerSecondVar;
    } modeB;

    //! Array of particles
    tParticle *_particles;

    // color modulate
    //    BOOL colorModulate;

    //! How many particles can be emitted per second
    float _emitCounter;

    //!  particle idx
    unsigned int _particleIdx;

    // Optimization
    //CC_UPDATE_PARTICLE_IMP    updateParticleImp;
    //SEL                        updateParticleSel;

    /** weak reference to the SpriteBatchNode that renders the Sprite */
    CC_PROPERTY(ParticleBatchNode*, _batchNode, BatchNode);

    // index of system in batch node array
    CC_SYNTHESIZE(unsigned int, _atlasIndex, AtlasIndex);

    //true if scaled or rotated
    bool _transformSystemDirty;
    // Number of allocated particles
    unsigned int _allocatedParticles;

    /** Is the emitter active */
    bool _isActive;
    /** Quantity of particles that are being simulated at the moment */
    CC_PROPERTY_READONLY(unsigned int, _particleCount, ParticleCount)
    /** How many seconds the emitter will run. -1 means 'forever' */
    CC_PROPERTY(float, _duration, Duration)
    /** sourcePosition of the emitter */
    CC_PROPERTY_PASS_BY_REF(Point, _sourcePosition, SourcePosition)
    /** Position variance of the emitter */
    CC_PROPERTY_PASS_BY_REF(Point, _posVar, PosVar)
    /** life, and life variation of each particle */
    CC_PROPERTY(float, _life, Life)
    /** life variance of each particle */
    CC_PROPERTY(float, _lifeVar, LifeVar)
    /** angle and angle variation of each particle */
    CC_PROPERTY(float, _angle, Angle)
    /** angle variance of each particle */
    CC_PROPERTY(float, _angleVar, AngleVar)

//////////////////////////////////////////////////////////////////////////
public:
    // mode A
    virtual const Point& getGravity();
    virtual void setGravity(const Point& g);
    virtual float getSpeed() const;
    virtual void setSpeed(float speed);
    virtual float getSpeedVar() const;
    virtual void setSpeedVar(float speed);
    virtual float getTangentialAccel() const;
    virtual void setTangentialAccel(float t);
    virtual float getTangentialAccelVar() const;
    virtual void setTangentialAccelVar(float t);
    virtual float getRadialAccel() const;
    virtual void setRadialAccel(float t);
    virtual float getRadialAccelVar() const;
    virtual void setRadialAccelVar(float t);
    virtual bool getRotationIsDir() const;
    virtual void setRotationIsDir(bool t);
    // mode B
    virtual float getStartRadius() const;
    virtual void setStartRadius(float startRadius);
    virtual float getStartRadiusVar() const;
    virtual void setStartRadiusVar(float startRadiusVar);
    virtual float getEndRadius() const;
    virtual void setEndRadius(float endRadius);
    virtual float getEndRadiusVar() const;
    virtual void setEndRadiusVar(float endRadiusVar);
    virtual float getRotatePerSecond() const;
    virtual void setRotatePerSecond(float degrees);
    virtual float getRotatePerSecondVar() const;
    virtual void setRotatePerSecondVar(float degrees);

    virtual void setScale(float s);
    virtual void setRotation(float newRotation);
    virtual void setScaleX(float newScaleX);
    virtual void setScaleY(float newScaleY);
    
    virtual bool isActive() const;
    virtual bool isBlendAdditive() const;
    virtual void setBlendAdditive(bool value);
//////////////////////////////////////////////////////////////////////////
    
    /** start size in pixels of each particle */
    CC_PROPERTY(float, _startSize, StartSize)
    /** size variance in pixels of each particle */
    CC_PROPERTY(float, _startSizeVar, StartSizeVar)
    /** end size in pixels of each particle */
    CC_PROPERTY(float, _endSize, EndSize)
    /** end size variance in pixels of each particle */
    CC_PROPERTY(float, _endSizeVar, EndSizeVar)
    /** start color of each particle */
    CC_PROPERTY_PASS_BY_REF(Color4F, _startColor, StartColor)
    /** start color variance of each particle */
    CC_PROPERTY_PASS_BY_REF(Color4F, _startColorVar, StartColorVar)
    /** end color and end color variation of each particle */
    CC_PROPERTY_PASS_BY_REF(Color4F, _endColor, EndColor)
    /** end color variance of each particle */
    CC_PROPERTY_PASS_BY_REF(Color4F, _endColorVar, EndColorVar)
    //* initial angle of each particle
    CC_PROPERTY(float, _startSpin, StartSpin)
    //* initial angle of each particle
    CC_PROPERTY(float, _startSpinVar, StartSpinVar)
    //* initial angle of each particle
    CC_PROPERTY(float, _endSpin, EndSpin)
    //* initial angle of each particle
    CC_PROPERTY(float, _endSpinVar, EndSpinVar)
    /** emission rate of the particles */
    CC_PROPERTY(float, _emissionRate, EmissionRate)
    /** maximum particles of the system */
    CC_PROPERTY(unsigned int, _totalParticles, TotalParticles)
    /** conforms to CocosNodeTexture protocol */
    CC_PROPERTY(Texture2D*, _texture, Texture)
    /** conforms to CocosNodeTexture protocol */
    CC_PROPERTY_PASS_BY_REF(BlendFunc, _blendFunc, BlendFunc)
    /** does the alpha value modify color */
    CC_PROPERTY(bool, _opacityModifyRGB, OpacityModifyRGB)

    /** whether or not the particles are using blend additive.
    If enabled, the following blending function will be used.
    @code
    source blend function = GL_SRC_ALPHA;
    dest blend function = GL_ONE;
    @endcode
    */
    bool _isBlendAdditive;
    /** particles movement type: Free or Grouped
    @since v0.8
    */
    CC_PROPERTY(tPositionType, _positionType, PositionType)
    /** whether or not the node will be auto-removed when it has no particles left.
    By default it is false.
    @since v0.8
    */
protected:
    bool _isAutoRemoveOnFinish;
public:
    virtual bool isAutoRemoveOnFinish() const;
    virtual void setAutoRemoveOnFinish(bool var);

    /** Switch between different kind of emitter modes:
    - kParticleModeGravity: uses gravity, speed, radial and tangential acceleration
    - kParticleModeRadius: uses radius movement + rotation
    */
    CC_PROPERTY(int, _emitterMode, EmitterMode)

public:
    ParticleSystem();
    virtual ~ParticleSystem();

    /** creates an initializes a ParticleSystem from a plist file.
    This plist files can be created manually or with Particle Designer:
    http://particledesigner.71squared.com/
    @since v2.0
    */
    static ParticleSystem * create(const char *plistFile);

    //! create a system with a fixed number of particles
    static ParticleSystem* createWithTotalParticles(unsigned int numberOfParticles);

    /** initializes a ParticleSystem*/
    bool init();
    /** initializes a ParticleSystem from a plist file.
    This plist files can be created manually or with Particle Designer:
    http://particledesigner.71squared.com/
    @since v0.99.3
    */
    bool initWithFile(const char *plistFile);

    /** initializes a QuadParticleSystem from a Dictionary.
    @since v0.99.3
    */
    bool initWithDictionary(Dictionary *dictionary);
    
    /** initializes a particle system from a NSDictionary and the path from where to load the png
     @since v2.1
     */
    bool initWithDictionary(Dictionary *dictionary, const char *dirname);

    //! Initializes a system with a fixed number of particles
    virtual bool initWithTotalParticles(unsigned int numberOfParticles);
    //! Add a particle to the emitter
    bool addParticle();
    //! Initializes a particle
    void initParticle(tParticle* particle);
    //! stop emitting particles. Running particles will continue to run until they die
    void stopSystem();
    //! Kill all living particles.
    void resetSystem();
    //! whether or not the system is full
    bool isFull();

    //! should be overridden by subclasses
    virtual void updateQuadWithParticle(tParticle* particle, const Point& newPosition);
    //! should be overridden by subclasses
    virtual void postStep();

    virtual void update(float dt);
    virtual void updateWithNoTime(void);

protected:
    virtual void updateBlendFunc();
};

// end of particle_nodes group
/// @}

NS_CC_END

#endif //__CCPARTICLE_SYSTEM_H__
