/**
 * @file linearfilter.cpp
 *
 * @date   Feb 4, 2013
 * @author parallels
 * @brief \todo
 */

//License File

//****************SYSTEM DEPENDANCIES**************************//
//*****************LOCAL DEPENDANCIES**************************//
#include<kvh_driver/linearfilter.h>
//**********************NAMESPACES*****************************//

using namespace kvh_driver;

LinearFilter::LinearFilter(int state_size, int input_size, int measurment_size, const ColumnVector& sys_noise_mu, const SymmetricMatrix& sys_noise_cov, const Matrix& A, const Matrix& B, const ColumnVector& measurement_noise_mu, const SymmetricMatrix& measurement_noise_cov, const Matrix& H):
														state_size_(state_size),
														input_size_(input_size),
														measurement_size_(measurment_size),
														AB_(2),
														sys_pdf_(NULL),
														sys_model_(NULL),
														mes_pdf_(NULL),
														mes_model_(NULL),
														filter_init_(false),
														filter_(NULL),
														prior_(NULL)
{
	//Build System Evolution Vector
	this->AB_[0]   = A;
	this->AB_[1]   = B;


	//Build System PDF/Model
	Gaussian system_uncertainty(sys_noise_mu, sys_noise_cov);
	this->sys_pdf_   = new LinearAnalyticConditionalGaussian(this->AB_, system_uncertainty);
	this->sys_model_ = new LinearAnalyticSystemModelGaussianUncertainty(this->sys_pdf_);

	//Build Measurement PDF/Model
	Gaussian measurement_uncertainty(measurement_noise_mu, measurement_noise_cov);
	this->mes_pdf_   = new LinearAnalyticConditionalGaussian(H, measurement_uncertainty);
	this->mes_model_ = new LinearAnalyticMeasurementModelGaussianUncertainty(this->mes_pdf_);
}

LinearFilter::~LinearFilter()
{
	if(this->sys_model_!= NULL) delete this->sys_model_;
	if(this->sys_pdf_  != NULL) delete this->sys_pdf_;
	if(this->mes_model_!= NULL) delete this->mes_model_;
	if(this->mes_pdf_  != NULL) delete this->mes_pdf_;
	if(this->filter_   != NULL) delete this->filter_;
	if(this->prior_    != NULL) delete this->prior_;
}

bool LinearFilter::init(const ColumnVector& initial_state, const SymmetricMatrix& initial_covar)
{
	//Check to make sure sizes match up
	if(initial_state.size() == this->state_size_ && initial_covar.size1()==this->state_size_)
	{
		this->prior_       = new Gaussian(initial_state, initial_covar);
		this->filter_      = new ExtendedKalmanFilter(this->prior_);
		this->filter_init_ = true;
		return true;
	}
	else
	{
		ROS_ERROR("Cannot Initialize Filter with State/Covar size %d/%d, Expecting Size %d/%d", initial_state.size(), initial_covar.size1(), constants::ODOM_STATE_SIZE(), constants::ODOM_STATE_SIZE());
		return false;
	}
}

bool LinearFilter::update(const ColumnVector& input, const ColumnVector& measurement)
{
	if(this->filter_init_)
	{
		//Check to make sure sizes match up
		if(input.size() == this->input_size_ && measurement.size()==this->measurement_size_)
		{
			this->filter_->Update(this->sys_model_, input, this->mes_model_, measurement);
			this->prior_ = this->filter_->PostGet();
			return true;
		}
		else
		{
			ROS_ERROR("Cannot update filter with input size %d and measurement size %d, expecting %d, %d", input.size(), measurement.size(), constants::INPUT_SIZE(), constants::MEASUREMENT_SIZE());
			return false;
		}
	}
	else
	{
		ROS_ERROR("Cannot perform Filter update on uninitialized filter!!");
		return false;
	}
}

bool LinearFilter::getEstimate(ColumnVector& state_estimate, SymmetricMatrix& covar) const
{
	if(this->filter_init_)
	{
		state_estimate = this->prior_->ExpectedValueGet();
		covar          = this->prior_->CovarianceGet();
		return true;
	}
	else
	{
		ROS_ERROR("Cannot get estimate on uninitialized filter!!");
		return false;
	}
}

bool LinearFilter::isInitialized()
{
	return this->filter_init_;
}
