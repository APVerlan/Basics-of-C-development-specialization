#include "Common.h"
#include <algorithm>

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`
class Rectangle : public IShape {
public:
	unique_ptr<IShape> Clone() const { return make_unique<Rectangle>(*this); }

	void SetPosition(Point point) { position_ = point; }
	Point GetPosition() const { return position_; }

	void SetSize(Size size) { size_ = size; }
	Size GetSize() const { return size_; }

	void SetTexture(shared_ptr<ITexture> texture) { texture_ = texture; }
	ITexture* GetTexture() const { return texture_.get(); }

	// Рисует фигуру на указанном изображении
	void Draw(Image& image) const {
		// точка вне изображения
		if (position_.y > image.size() || position_.x > image[0].size())
			return;

		// пересечение фигуры и изображения
		Size direct_size = { min(size_.width, static_cast<int>(image[0].size()) - position_.x),
			min(size_.height, static_cast<int>(image.size()) - position_.y)
		};

		// заливаем по умолчанию '.'
		for (size_t i = position_.x; i < position_.x + direct_size.width; ++i) {
			for (size_t j = position_.y; j < position_.y + direct_size.height; ++j) {
				image[j][i] = '.';
			}
		}

		if (texture_) {
			// пересечение текстуры и фигуры
			Size dirext_texture_size = { min(direct_size.width, texture_->GetSize().width),
			min(direct_size.height, texture_->GetSize().height)
			};

			for (size_t i = position_.x; i < position_.x + dirext_texture_size.width; ++i) {
				for (size_t j = position_.y; j < position_.y + dirext_texture_size.height; ++j) {
					image[j][i] = texture_->GetImage()[j - position_.y][i - position_.x];
				}
			}
		}

	}

private:
	shared_ptr<ITexture> texture_;
	Size size_;
	Point position_;
};

class Ellipse : public IShape {
public:
	unique_ptr<IShape> Clone() const { return make_unique<Ellipse>(*this); }

	void SetPosition(Point point) { position_ = point; }
	Point GetPosition() const { return position_; }

	void SetSize(Size size) { size_ = size; }
	Size GetSize() const { return size_; }

	void SetTexture(shared_ptr<ITexture> texture) { texture_ = texture; }
	ITexture* GetTexture() const { return texture_.get(); }

	void Draw(Image& image) const {
		// точка вне изображения
		if (position_.y > image.size() || position_.x > image[0].size())
			return;

		// пересечение фигуры и изображения
		Size direct_size = { min(size_.width, static_cast<int>(image[0].size()) - position_.x),
			min(size_.height, static_cast<int>(image.size()) - position_.y)
		};

		// заливаем по умолчанию '.'
		for (int i = position_.x; i < position_.x + direct_size.width; ++i) {
			for (int j = position_.y; j < position_.y + direct_size.height; ++j) {
				if (IsPointInEllipse({i - position_.x, j - position_.y}, size_))
					image[j][i] = '.';
			}
		}

		if (texture_) {
			// пересечение текстуры и фигуры
			Size dirext_texture_size = { min(direct_size.width, texture_->GetSize().width),
			min(direct_size.height, texture_->GetSize().height)
			};

			for (int i = position_.x; i < position_.x + dirext_texture_size.width; ++i) {
				for (int j = position_.y; j < position_.y + dirext_texture_size.height; ++j) {
					if (IsPointInEllipse({ i - position_.x, j - position_.y }, size_))
						image[j][i] = texture_->GetImage()[j - position_.y][i - position_.x];
				}
			}
		}
	}
private:
	shared_ptr<ITexture> texture_;
	Size size_;
	Point position_;
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	if (shape_type == ShapeType::Rectangle)
		return make_unique<Rectangle>();
	else if (shape_type == ShapeType::Ellipse)
		return make_unique<Ellipse>();
}