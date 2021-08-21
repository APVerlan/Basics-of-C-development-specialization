namespace RAII {
	template <typename Provider>
	class Booking {
	public:
		Booking(Provider* provider, int& counter) : 
			provider_(provider),
			id_(counter) 
		{
		}

		Booking(const Booking&) = delete;
		Booking(Booking&& other) : provider_(other.provider_), id_(other.id_) {
			other.provider_ = nullptr;
		};

		Booking& operator=(const Booking&) = delete;
		Booking& operator=(Booking&& other) {
			provider_ = other.provider_;
			id_ = other.id_;
			other.provider_ = nullptr;

			return *this;
		}

		~Booking() { 
			if (provider_)
				provider_->CancelOrComplete(*this); 
		}
	private:
		Provider* provider_;
		int id_;
	};
}