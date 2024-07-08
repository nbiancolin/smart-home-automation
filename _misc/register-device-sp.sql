-- ================================================
-- Template generated from Template Explorer using:
-- Create Procedure (New Menu).SQL
--
-- Use the Specify Values for Template Parameters 
-- command (Ctrl-Shift-M) to fill in the parameter 
-- values below.
--
-- This block of comments will not be included in
-- the definition of the procedure.
-- ================================================
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Nicholas Biancolin>
-- Create date: <July 7th, 2024>
-- Description:	<Called by an arduino the first time it powers up>
-- =============================================
CREATE PROCEDURE [dbo].[REGISTER_DEVICE]
	@DISPLAY_NAME varchar(64),
	@DEVICE_TYPE varchar(64),
	@ADDRESS varchar(16)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    -- Check if a device with the given "display name" exists already
	IF EXISTS (SELECT 1 FROM dbo.devices WHERE DISPLAY_NAME = @DISPLAY_NAME)
	BEGIN
		-- give that device its new IP
		UPDATE dbo.devices SET [ADDRESS] = @ADDRESS WHERE DISPLAY_NAME = @DISPLAY_NAME;
	END
	ELSE
	BEGIN
		INSERT INTO dbo.devices (DISPLAY_NAME, DEVICE_TYPE, [ADDRESS]) VALUES (@DISPLAY_NAME, @DEVICE_TYPE, @ADDRESS);
	END
	
	SELECT SCOPE_IDENTITY();
END
GO
