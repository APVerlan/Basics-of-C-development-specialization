#include "test_runner.h"

#include "geo2d.h"
#include "game_object.h"

// Определите классы Unit, Building, Tower и Fence так, чтобы они наследовались от
// GameObject и реализовывали его интерфейс.

class Unit : public GameObject {
public:
	explicit Unit(geo2d::Point position) : point_(position) {};
	bool Collide(const GameObject& that) const;
	bool CollideWith(const Unit& that) const;
	bool CollideWith(const Building& that) const;
	bool CollideWith(const Tower& that) const;
	bool CollideWith(const Fence& that) const;

	geo2d::Point GetGeometryInterp_() const { return point_; };
private:
	geo2d::Point point_;
};

class Building : public GameObject {
public:
	explicit Building(geo2d::Rectangle geometry) : rect_(geometry) {};
	bool Collide(const GameObject& that) const;
	bool CollideWith(const Unit& that) const;
	bool CollideWith(const Building& that) const;
	bool CollideWith(const Tower& that) const;
	bool CollideWith(const Fence& that) const;

	geo2d::Rectangle GetGeometryInterp_() const { return rect_; };

private:
	geo2d::Rectangle rect_;
};

class Tower : public GameObject {
public:
	explicit Tower(geo2d::Circle geometry) : circle_(geometry) {};
	bool Collide(const GameObject& that) const;
	bool CollideWith(const Unit& that) const;
	bool CollideWith(const Building& that) const;
	bool CollideWith(const Tower& that) const;
	bool CollideWith(const Fence& that) const;

	geo2d::Circle GetGeometryInterp_() const { return circle_; };

private:
	geo2d::Circle circle_;
};

class Fence : public GameObject {
public:
	explicit Fence(geo2d::Segment geometry) : segm_(geometry) {};
	bool Collide(const GameObject& that) const;
	bool CollideWith(const Unit& that) const;
	bool CollideWith(const Building& that) const;
	bool CollideWith(const Tower& that) const;
	bool CollideWith(const Fence& that) const;

	geo2d::Segment GetGeometryInterp_() const { return segm_; };

private:
	geo2d::Segment segm_;
};



bool Unit::Collide(const GameObject& that) const {
	return that.CollideWith(*this);
}

bool Unit::CollideWith(const Unit& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Unit::CollideWith(const Building& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Unit::CollideWith(const Tower& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Unit::CollideWith(const Fence& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Building::Collide(const GameObject& that) const {
	return that.CollideWith(*this);
}

bool  Building::CollideWith(const Unit& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool  Building::CollideWith(const Building& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool  Building::CollideWith(const Tower& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool  Building::CollideWith(const Fence& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Tower::Collide(const GameObject& that) const {
	return that.CollideWith(*this);
}

bool Tower::CollideWith(const Unit& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Tower::CollideWith(const Building& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Tower::CollideWith(const Tower& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Tower::CollideWith(const Fence& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Fence::Collide(const GameObject& that) const {
	return that.CollideWith(*this);
}

bool Fence::CollideWith(const Unit& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Fence::CollideWith(const Building& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Fence::CollideWith(const Tower& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};

bool Fence::CollideWith(const Fence& that) const {
	return geo2d::Collide(this->GetGeometryInterp_(),
		that.GetGeometryInterp_());
};